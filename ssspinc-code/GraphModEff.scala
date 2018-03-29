package graphinc

import org.apache.spark.graphx._
import utils.FileLogger


class GraphModEff(sourceVertex: VertexId) extends java.io.Serializable {

  type Vattr = (Boolean, Double)
  private def incrementMap(dist: Double): Double = dist + 1

  private def mergeMsgs(path1: Double, path2: Double): Double = {
    if (path1 < path2)
      return path1
    else
      return path2

  }

  def vertexProgram(id: VertexId, attr: Vattr, path: Double): Vattr = {
    val newDist = if (id == sourceVertex) 0.0 else math.min  (attr._2, path)
    return (
      false,
      newDist)
  }

  def sendMessage(edge: EdgeTriplet[Vattr, Double]): Iterator[(VertexId, Double)] = {
    if (edge.srcAttr._2 + edge.attr < edge.dstAttr._2 )
      return Iterator((edge.dstId, edge.srcAttr._2 + edge.attr))
    else {
      return Iterator.empty
    }
    return Iterator.empty
  }


  def initVattr(gr: Graph[Int, Double]): Graph[Vattr, Double] = {
    val initVertexMsg = (false, Double.MaxValue)
    val initVertexMsgSource =  (true, 0.0)
    val setVertexAttr = (vid: VertexId, vdata: Int) => if (vid == sourceVertex) initVertexMsgSource else initVertexMsg
    return gr.mapVertices(setVertexAttr)
  }



  def run(graph: Graph[Vattr, Double],
          dbg: Boolean = false,
          maxIterations: Int = 10)
  : Graph[Vattr, Double] = {

    var g = graph
    //g.cache()
    var prevG: Graph[Vattr, Double] = null
    var i: Int = 0

    FileLogger.println("Im here.")
    var messages = g.mapReduceTriplets(sendMessage, mergeMsgs)
    var activeMessages = messages.count()
    var prevMessages = messages
    if (true) {
      FileLogger.println("Number of active messages:" + activeMessages)
    }
    while (i < maxIterations && activeMessages > 0) {
      prevG = g
      prevMessages = messages
      g = g.joinVertices(messages)(vertexProgram)

      val oldMessages = messages
      messages = g.mapReduceTriplets(sendMessage, mergeMsgs)


      activeMessages = messages.count()
      if (true) {
        FileLogger.println("Number of active messages:" + activeMessages)
        if (activeMessages < 3){
          val m = messages.collect()
          m.foreach{ e =>FileLogger.println("vertex: "+ e._1 + " value: " + e._2)}
        }
      }


      prevMessages.unpersist(blocking = false)
      prevG.unpersistVertices(blocking = false)
      prevG.edges.unpersist(blocking = false)
      prevG.unpersist(blocking = false)


      i += 1


    }
    return g
  }
}

