To execute:
  Run Spark master using $SPARK_HOME/sbin/start-master.sh 
  Run K Spark workers using $SPARK_HOME/sbin/start-slave.sh 
  Run ./sssp.sh with appropriate parameters
  
Example: 
  suppose master hostname is "192.168.0.1",
  input graph is "graph1",
  output shortest path is "path1",
  log file path is "log",
  new edges is "edge1",
  source vertex of sssp is 1, and
  maximum number of supersteps is 5. Then we can run the ssspinc algorithm using:
  
  
  ./sssp.sh 192.168.0.1 graph1 path1 log edge1 1 5  


  
