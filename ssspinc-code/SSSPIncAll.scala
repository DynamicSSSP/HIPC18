package graph

import graphinc._
import org.apache.log4j.{Level, Logger}
import org.apache.spark.graphx._
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.SQLContext
import org.apache.spark.{SparkConf, SparkContext}
import utils._


object SSSPIncAll {

  def main(args: Array[String]): Unit = {

    val filename = args(0)
    val output = args(1)
    val log = args(2)
    val modificationFile =args(3)
    val iterStr = args(4)
    val srcStr = args(5)

    FileLogger.open(log)


    try {
      val start = System.currentTimeMillis()

      val conf = new SparkConf().setAppName("ssspinc-shortestpath")
      conf.set("spark.scheduler.mode", "FAIR")
      conf.set("spark.memory.fraction", "0.6")
      conf.set("spark.eventLog.enabled", "true")
      val home = sys.env("HOME")
      conf.set("spark.eventLog.dir", home + "/spark/logs")
      FileLogger.println(conf.getAll.mkString("\n"))
      val sc = new SparkContext(conf)
      FileLogger.println("AppId:" + sc.applicationId)
      //sc.setLogLevel("ERROR")

      //Logger.getLogger("org").setLevel(Level.OFF)
      //Logger.getLogger("akka").setLevel(Level.OFF)

      val sqlC = new SQLContext(sc)

      val metaFiles = FileHandler.loadInput(filename)
      val uritype = metaFiles(0).uriType
     // val vFile = metaFiles(0).uri
      val eFile = metaFiles(0).uri

      FileLogger.println("Graph: " + eFile)

      val debug = false

      val metaFilesC = FileHandler.loadInput(modificationFile)
      val changesDF = Data.loadCSV(sqlC, metaFilesC(0).uri, false, false)

      FileLogger.println("Changes: " + metaFilesC(0).uri)

      val aEdges : RDD[Edge[Double]] = changesDF.rdd.map(r=> Edge(r.apply(0).asInstanceOf[String].toLong, r.apply(1).asInstanceOf[String].toLong,
              r.apply(2).asInstanceOf[String].toDouble))
        .filter(e=>e.attr >= 1).map(e=>Edge(e.srcId, e.dstId, e.attr * -1.0))

      //val revEdges = aEdges.map(e=> Edge(e.dstId, e.srcId, e.attr))
      val addedEdges = aEdges //aEdges.union(revEdges)
      FileLogger.println("Number of added edges:" + addedEdges.count())







      //val vertexRDD = sc.objectFile[(VertexId, Int )](vFile)
      //val edgeRDD = sc.objectFile[org.apache.spark.graphx.Edge[Int]](eFile).map(e=>Edge(e.srcId, e.dstId, e.attr.toDouble))

      /*val aEdges = edgeRDD.sample(false, 0.1).map{case e=>Edge(e.srcId, e.dstId, -1.0 * (e.attr + new Random().nextInt(10)))}
      val revEdges = aEdges.map(e=> Edge(e.dstId, e.srcId, e.attr))
      val addedEdges = aEdges.union(revEdges)
      FileLogger.println("Number of added edges:" + addedEdges.count())
      */


      val sourceVertex = srcStr.toLong
      val iteration = iterStr.toInt

      val graph = GraphLoader.edgeListFile(sc, eFile).partitionBy(PartitionStrategy.CanonicalRandomVertexCut)
        .groupEdges((attr1, attr2)=> attr1 ).mapEdges(e=>e.attr.toDouble)



      //val graph = Graph(vertexRDD, edgeRDD).partitionBy(PartitionStrategy.EdgePartition2D, 240)
      //  .groupEdges((attr1, attr2)=>(attr1+attr2) ).mapEdges(e=>e.attr.toDouble)


      val stop2 = System.currentTimeMillis();

      FileLogger.println("Init successfully done in " + ((stop2 - start) / 1000.0) + " sec" );


      val gmod = new GraphModEff(sourceVertex)
      val grInitialized = gmod.initVattr(graph)

      val grSD = gmod.run(grInitialized,debug, iteration)





      val gmstop = System.currentTimeMillis();


      FileLogger.println("GraphInc first round successfully done in " + ((gmstop - stop2 ) / 1000.0) + " sec" );
      //val gr3 = gmod.resetGraph(grSD)



      val attrV = grSD.vertices


      val dstVertices = addedEdges.map(e=>e.dstId).cache()
      val srcVertices = addedEdges.map(e=>e.srcId).cache()
      //val dstVerticesB  = sc.broadcast(dstVertices.collect().toSet)



      val vertexIDs = graph.vertices.map(v=>v._1).cache()

      val unAfftectedButNew = dstVertices.subtract(srcVertices.union(vertexIDs)).map(vid=>(vid, (false,Double.MaxValue)))
      val affectedButNew = srcVertices.subtract(vertexIDs).map(vid=>(vid, (true,Double.MaxValue)))


      vertexIDs.unpersist(false)
      dstVertices.unpersist(false)
      srcVertices.unpersist(false)

      val remCount = affectedButNew.count()
      val uaffC = unAfftectedButNew.count()
      val remStop = System.currentTimeMillis();



      FileLogger.println("Computing nodes not in src or vertexrdd successfully done in " + ((remStop - gmstop) / 1000.0) + " sec" );
      FileLogger.println("")



      val distances = grSD.mapVertices{case (vid, vattr) => vattr._2}.vertices




      val srcVerticesB = sc.broadcast(srcVertices.collect().toSet)
      val dstVerticesB = sc.broadcast(dstVertices.collect().toSet)



      val updatedV = gmod.initVattr(graph).joinVertices(grSD.vertices){case (vid,vattr,vattr2)=>
        if (srcVerticesB.value.contains(vid))
          (true,
            vattr2._2
            )
        else
          (
          false,  // affected
         vattr2._2 // distSoFar : Double,
         )}.vertices.union(affectedButNew.union(unAfftectedButNew))

      val uc = updatedV.count();

      val allEdges = grSD.edges.union(addedEdges)

      grSD.edges.unpersist(false)
      grSD.vertices.unpersist(false)
      grSD.unpersist(false)

      val completeGraph = Graph(updatedV, allEdges).partitionBy(PartitionStrategy.CanonicalRandomVertexCut)
          .groupEdges((att1, att2) => if (att1 < 0.0) att1 else att2).mapEdges(e=>if (e.attr < 0) -e.attr else e.attr )




      if (debug) completeGraph.edges.collect().foreach(e=>FileLogger.println(e.srcId + " " + e.dstId + " : " + e.attr))

      val stop3 = System.currentTimeMillis();
      FileLogger.println("totat vertices:"+ uc)
      FileLogger.println("Data preparation for the increment done in " + ((stop3 - gmstop) / 1000.0) + " sec" );




      val grSD2 = gmod.run(completeGraph,debug, iteration)

      val stop = System.currentTimeMillis();
      val stop4 = System.currentTimeMillis();



      val distRDD = grSD2.vertices.map{ case (v,a)=> (v, if (a._2>1000.0) 0 else a._2) }

      val totalDistance = distRDD.map{case (vid,d)=>d}.reduce(_+_)

      FileLogger.println("Total Distance after changes = " + totalDistance)


      FileLogger.println("GraphInc second round successfully done in " + ((stop4- stop3) / 1000.0) + " sec" );




      /*
      val newGraph = Graph(vertexRDD, allEdges).partitionBy(PartitionStrategy.EdgePartition2D, 96).
          groupEdges((att1, att2) => if (att1 < 0.0) att1 else att2).mapEdges(e=>if (e.attr < 0) -e.attr else e.attr )



      if (debug) newGraph.edges.collect().foreach(e=>FileLogger.println(e.srcId + " " + e.dstId + " : " + e.attr))
      val totalWeight = newGraph.edges.map(e=>e.attr).reduce(_+_)

      FileLogger.println("Total edge weight: " + totalWeight)
      val res = ShortestPaths.run(newGraph, Array(sourceVertex))
      val rdd: Graph[Double,Double] = res.mapVertices{ case (id, m) => m.getOrElse(sourceVertex,0).toDouble}

      val totalDFSDistance = rdd.vertices.map{case (vid,m)=>m}.reduce(_+_)
      FileLogger.println("Total SSSP:" + totalDFSDistance)
      //val changesGraph = Graph(vertexRDD, addedEdges)

      val sspStop = System.currentTimeMillis();

      FileLogger.println("SSSP successfully done in " + ((sspStop - stop4) / 1000.0) + " sec" );
      */

      //val joinDist: RDD[(VertexId,Double,Double)]  = rdd.vertices.join(distRDD).map{case (vid, (s, g))=> (vid,s,g) }

      //val diffs = joinDist.filter{case (vid, s,g) => s != g}.collect()
      //FileLogger.println("Num of diffs: " + diffs.size)
      //diffs.foreach{case (vid, s ,g) => FileLogger.println(vid, s, g )}

      // .partitionBy(PartitionStrategy.EdgePartition2D, 200).cache()

      //FileLogger.println("final edges:")
      //addedEdges.collect().foreach(FileLogger.println(_))






      val distFile = FileHandler.getFullName(sc, uritype, "distance")



      //val res = ShortestPaths.run(completeGraph, Array(sourceVertex))
      //val rdd = res.vertices.map { case (id, m) => (id, m.getOrElse(sourceVertex,0))}

      //val stop5 = System.currentTimeMillis();
      //FileLogger.println("SSSP successfully done in " + ((stop5 - stop4) / 1000.0) + " sec" );



      //FileLogger.println("SSSP:")
      //rdd.take(10).foreach(FileLogger.println(_))

      //val totalDFSDistance = rdd.map{case (vid,m)=> m}.reduce(_+_)
      //FileLogger.println("Total SSSP:" + totalDFSDistance)
      //if (outputType == "csv") {


      val schemaString = "vertex, distance"
      val df = sqlC.createDataFrame(distRDD).toDF("vertex", "distanceGI")
      FileLogger.println("Scheme string: " + schemaString)
      //df.write
      //  .format("com.databricks.spark.csv")
      //  .option("header", "true")
      //  .save(distFile)
      //df.take(50).foreach(FileLogger.println(_))
      //ToCSV.fromVertexRDD(sqlC, degree, degreeFile)


      val objects = new Array[MetaFile](1)
      val verObj = new MetaFile
      verObj.objName = "CSV[vertex,distanceSSSP,distanceGI]"
      verObj.uri = distFile
      verObj.uriType = uritype

      objects.update(0, verObj)
      FileHandler.dumpOutput(output, objects)


      FileLogger.println("SSSPInc successfully done in " + ((stop - start) / 1000.0) + " sec" );
    }
    catch {
      case e: Exception => FileLogger.println(GalaxyException.getString(e))
    } finally {
      FileLogger.close();
    }
  }
}
