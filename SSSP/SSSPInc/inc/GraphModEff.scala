package inc

import org.apache.spark.graphx._


class GraphModEff(sourceVertex: VertexId) extends java.io.Serializable {

  type Vattr = (Boolean, Double) //affected Nodes, Shortest Path

  private def mergeMsgs(path1: Double, path2: Double): Double = {
    if (path1 < path2) path1 else path2
  }

  def vertexProgram(id: VertexId, attr: Vattr, path: Double): Vattr = {
    val newDist = if (id == sourceVertex) 0.0 else math.min  (attr._2, path)
    (false, newDist)
  }


  def sendMessage(edge: EdgeTriplet[Vattr, Double]): Iterator[(VertexId, Double)] = {
    if (edge.srcAttr._2 + edge.attr < edge.dstAttr._2 )
      Iterator((edge.dstId, edge.srcAttr._2 + edge.attr))
    else {
      Iterator.empty
    }
  }


  def sendMessageFirst(edge: EdgeTriplet[Vattr, Double]): Iterator[(VertexId, Double)] = {
    if (edge.srcAttr._1 && edge.srcAttr._2 + edge.attr < edge.dstAttr._2 )
      Iterator((edge.dstId, edge.srcAttr._2 + edge.attr))
    else {
      Iterator.empty
    }
  }


  def initVattr(gr: Graph[Int, Double]): Graph[Vattr, Double] = {
    val initVertexMsg = (false, Double.MaxValue)
    val initVertexMsgSource =  (true, 0.0)
    val setVertexAttr = (vid: VertexId, vdata: Int) => if (vid == sourceVertex) initVertexMsgSource else initVertexMsg
    gr.mapVertices(setVertexAttr)
  }




  def run(graph: Graph[Vattr, Double], dbg: Boolean = false, maxIterations: Int = 10) : Graph[Vattr, Double] = {
    var g = graph
    var prevG: Graph[Vattr, Double] = null
    var i: Int = 0
    var messages = g.mapReduceTriplets(sendMessageFirst, mergeMsgs)
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

