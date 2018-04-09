#!/bin/sh

$SPARK_HOME/bin/spark-submit --packages com.databricks:spark-csv_2.10:1.3.0,it.unipd.dei:graphx-diameter_2.10:0.1.0 --class $2 --executor-cores 20 --driver-memory 15G --executor-memory 120G --conf "spark.akka.frameSize=2047" --master spark://$1:7077 ssspinc.jar $3 $4 $5 $6 $7 $8 
