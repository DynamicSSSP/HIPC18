# Parallel Algorithms for Updating Single Source Shortest Paths in Large Dynamic Networks

## Getting Started
### External Softawre Prerequisites:
* Galois 2.2.1 [Webpage](http://iss.ices.utexas.edu/projects/galois/downloads/Galois-2.2.1.tar.gz)
  * Prerequisites: Boost, C++ compiler
* Spark 1.6.2 [Webpage](https://spark.apache.org/docs/1.6.2/)
  * Prerequisites: Java 1.8, Scala 2.10
 

#### Included in this repository:
* RMAT graph generator (Copyright (C) 2010 Mahantesh Halappanavar)

## Compiling

### Shared-memory SSSP update code

To build the shared-memory code, go to the SSSP/SSSPshmem directory and type make. You may need to adjust the Makefile for your compiler if you do not have g++.

### Distributed-memory SSSP update code

A prebuild of the scala code is provided in the scripts directory. To run the code follow the readMe file in the scripts directory.




