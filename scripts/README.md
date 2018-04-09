To execute:

  Download https://archive.apache.org/dist/spark/spark-1.6.2/spark-1.6.2-bin-hadoop2.6.tgz
  
  Set SPARK_HOME to the home directory of Spark-1.6-2:

    export SPARK_HOME=/path/to/spark-1.2.6

  Run Spark master:
    
    $SPARK_HOME/sbin/start-master.sh 
  
  Run K Spark workers: 
    
    $SPARK_HOME/sbin/start-slave.sh master-hostname:7077
  
  Run ./ssspinc.sh or ./ssspbase.sh with appropriate arguments (arguments to the ssspinc and ssspbase are the same)
  
Example: 
  suppose master hostname is "192.168.0.1",
  input graph is "karate.txt",
  output shortest path is "paths",
  log file path is "log",
  new edges is "karate_10-changes.txt",
  source vertex of sssp is 1, and
  maximum number of supersteps is 5. Then we can run the ssspinc algorithm using:
  
    bash ssspinc.sh 192.168.0.1 ../datasets/karate.txt paths log ../datasets/karate_10-changes.txt 5 1  


  
