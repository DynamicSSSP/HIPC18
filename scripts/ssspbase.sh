#!/bin/sh

hostname=$1
inputGraph=$2
outputSP=$3
log=$4
addedEdges=$5
num_iter=50
source_vertex=8
./run_spark_app.sh $hostname inc.SSSPSeqAll $inputGraph $outputSP $log $addedEdges $num_iter $source_vertex
