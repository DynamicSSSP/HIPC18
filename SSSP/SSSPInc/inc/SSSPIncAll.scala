package inc

import org.apache.spark.graphx._
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.{DataFrame, SQLContext}
import org.apache.spark.{SparkConf, SparkContext}


object SSSPIncAll {
  def loadCSV(sql:SQLContext, filepath: String, hasHeader: Boolean, inferSchema: Boolean) : DataFrame = {
    val df = sql.read
      .format("com.databricks.spark.csv")
      .option("header", hasHeader.toString) // Use first line of all files as header
      .option("inferSchema", inferSchema.toString)
      .load(filepath);
    df
  }

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

      val sqlC = new SQLContext(sc)
      val eFile = filename
      FileLogger.println("Graph: " + eFile)

      val debug = false

      val changesDF = loadCSV(sqlC, modificationFile, false, false)

      FileLogger.println("Changes: " + modificationFile)

      val aEdges : RDD[Edge[Double]] = changesDF.rdd.map(r=> Edge(r.apply(0).asInstanceOf[String].toLong, r.apply(1).asInstanceOf[String].toLong,
              r.apply(2).asInstanceOf[String].toDouble))
        .filter(e=>e.attr >= 1).map(e=>Edge(e.srcId, e.dstId, e.attr * -1.0))

      val addedEdges = aEdges //aEdges.union(revEdges)
      FileLogger.println("Number of added edges:" + addedEdges.count())



      val sourceVertex = srcStr.toLong
      val iteration = iterStr.toInt

      val graph = GraphLoader.edgeListFile(sc, eFile).partitionBy(PartitionStrategy.CanonicalRandomVertexCut)
        .groupEdges((attr1, attr2)=> attr1 ).mapEdges(e=>e.attr.toDouble)


      val stop2 = System.currentTimeMillis();

      FileLogger.println("Init successfully done in " + ((stop2 - start) / 1000.0) + " sec" );


      val gmod = new GraphModEff(sourceVertex)
      val grInitialized = gmod.initVattr(graph)

      val grSD = gmod.run(grInitialized,debug, iteration)



      val gmstop = System.currentTimeMillis();


      FileLogger.println("SSSPIncInc first round successfully done in " + ((gmstop - stop2 ) / 1000.0) + " sec" );



      val attrV = grSD.vertices

      val vertexIDs = graph.vertices.map(v=>v._1).cache()


      val dstVertices = addedEdges.map(e=>e.dstId).cache()
      val srcVertices = addedEdges.map(e=>e.srcId).cache()

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
          ( true, // affected
            vattr2._2 //shortest path
            )
        else
          (
          false,  // affected
          vattr2._2 // shortest path ,
         )}.vertices.union(affectedButNew.union(unAfftectedButNew))

      val uc = updatedV.count();



      grSD.edges.unpersist(false)
      grSD.vertices.unpersist(false)
      grSD.unpersist(false)

      val allEdges = grSD.edges.union(addedEdges)

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


      FileLogger.println("SSSPIncInc second round successfully done in " + ((stop4- stop3) / 1000.0) + " sec" );



      val distFile = output



      val schemaString = "vertex, distance"
      val df = sqlC.createDataFrame(distRDD).toDF("vertex", "distanceGI")
      FileLogger.println("Scheme string: " + schemaString)
      df.write
        .format("com.databricks.spark.csv")
        .option("header", "true")
        .save(distFile)




      FileLogger.println("SSSPInc successfully done in " + ((stop - start) / 1000.0) + " sec" );
    }
    catch {
      case e: Exception => FileLogger.println(e)
    } finally {
      FileLogger.close();
    }
  }
}
