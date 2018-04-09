package inc

import org.apache.spark.graphx._
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.{DataFrame, SQLContext}
import org.apache.spark.{SparkConf, SparkContext}


object SSSPSeqAll {
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

      val conf = new SparkConf().setAppName("ssspseq-shortestpath")
      conf.set("spark.scheduler.mode", "FAIR")
      conf.set("spark.memory.fraction", "0.6")
      conf.set("spark.eventLog.enabled", "true")
      val home = sys.env("HOME")
      conf.set("spark.eventLog.dir", home + "/spark/logs")
      FileLogger.println(conf.getAll.mkString("\n"))

      val sc = new SparkContext(conf)
      FileLogger.println("AppId:" + sc.applicationId)

      val sqlC = new SQLContext(sc)
      val edgeFile = filename
      FileLogger.println("Graph: " + edgeFile)

      val debug = false
      val changesDF = loadCSV(sqlC, modificationFile, false, false)
      FileLogger.println("Changes: " + modificationFile)
      val aEdges : RDD[Edge[Double]] = changesDF.rdd.map(r=> Edge(r.apply(0).asInstanceOf[String].toLong, r.apply(1).asInstanceOf[String].toLong,
              r.apply(2).asInstanceOf[String].toDouble))
        .filter(e=>e.attr >= 1).map(e=>Edge(e.srcId, e.dstId, e.attr * -1.0))

      val addedEdges = aEdges //aEdges.union(revEdges)
      FileLogger.println("Number of added edges:" + addedEdges.count())


     val graph = GraphLoader.edgeListFile(sc, edgeFile).partitionBy(PartitionStrategy.CanonicalRandomVertexCut)
        .groupEdges((attr1, attr2)=>attr1 ).mapEdges(e=>e.attr.toDouble)


      graph.edges.count()


      val sourceVertex = srcStr.toLong
      val iteration = iterStr.toInt



      val stop2 = System.currentTimeMillis();

      FileLogger.println("Init successfully done in " + ((stop2 - start) / 1000.0) + " sec" );


      val gmod = new GraphModEff(sourceVertex)
      val grInitialized = gmod.initVattr(graph)

      graph.vertices.unpersist(blocking = false)
      graph.edges.unpersist(blocking = false)

      graph.unpersist(blocking = false)

      val grSD = gmod.run(grInitialized,debug, iteration)


      val gmstop = System.currentTimeMillis();


      FileLogger.println("SSSPBase first round successfully done in " + ((gmstop - stop2 ) / 1000.0) + " sec" );


      val attrV = grSD.vertices


      val dstVertices = addedEdges.map(e=>(e.dstId, 1))
      val srcVertices = addedEdges.map(e=>(e.srcId, 1))

      val allVertices = dstVertices.union(srcVertices)

      val allEdges = grSD.edges.union(addedEdges)

      grSD.edges.unpersist(false)
      grSD.vertices.unpersist(false)
      grSD.unpersist(false)

      val nextGraph = Graph(allVertices, allEdges).partitionBy(PartitionStrategy.CanonicalRandomVertexCut)
          .groupEdges((att1, att2) => if (att1 < 0.0) att1 else att2).mapEdges(e=>if (e.attr < 0) -e.attr else e.attr )

      val uc = nextGraph.edges.count();



     val stop3 = System.currentTimeMillis();
      FileLogger.println("Second run preparation for the increment done in " + ((stop3 - gmstop) / 1000.0) + " sec" );


      val gmod2 = new GraphModEff(sourceVertex)
      val grInitialized2 = gmod2.initVattr(nextGraph)
      val grSD2 = gmod2.run(grInitialized2,debug, iteration)


      val stop = System.currentTimeMillis();
      val stop4 = System.currentTimeMillis();


      FileLogger.println("SSSPBase second round successfully done in " + ((stop4 - stop3) / 1000.0) + " sec" );

      val distRDD = grSD2.vertices.map{ case (v,a)=> (v, if (a._2>1000.0) 0 else a._2) }

      val totalDistance = distRDD.map{case (vid,d)=>d}.reduce(_+_)

      FileLogger.println("Total Distance after changes = " + totalDistance)







      val distFile = output
      val schemaString = "vertex, distance"
      val df = sqlC.createDataFrame(distRDD).toDF("vertex", "distanceGI")
      FileLogger.println("Scheme string: " + schemaString)
      df.write
        .format("com.databricks.spark.csv")
        .option("header", "true")
        .save(distFile)

      FileLogger.println("SSSPBase successfully done in " + ((stop - start) / 1000.0) + " sec" );
    }
    catch {
      case e: Exception => FileLogger.println(e)
    } finally {
      FileLogger.close();
    }
  }
}
