#!/bin/sh

hostname=$1
inputGraph=$2
outputSP=$3
log=$4
addedEdges=$5
num_iter=50
source_vertex=8
./run_spark.sh $hostname graph.SSSPIncAll $inputGraph $outputSP $log $addedEdges $num_iter $source_vertex
