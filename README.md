# Parallel Algorithms for Updating Single Source Shortest Paths in Large Dynamic Networks

## Getting Started
### External Softawre Prerequisites:
* Galois 2.2.1 [Webpage](http://iss.ices.utexas.edu/projects/galois/downloads/Galois-2.2.1.tar.gz)
* Spark 1.6.2 [Webpage](https://spark.apache.org/docs/1.6.2/)

#### Included in this repository:
* RMAT graph generator (Copyright (C) 2010 Mahantesh Halappanavar)

### Installation
Following are the steps to setup RMAT network and real network for running Dynamic SSSP and Galois
1. Compile RMAT graph generator by issuing "make" command (without quotes).

2. Generate RMAT networks for scale 24 for different probablities, by issuing following commands 
"./driverForRmat 24 24 0.25 0.25 0.25 0.25 data/RMAT24.er"
"./driverForRmat 24 24 0.45 0.15 0.15 0.25 data/RMAT24.gr"

3. For real world networks (https://snap.stanford.edu/data/), following command were used from unix machine-
"wget https://snap.stanford.edu/data/com-LiveJournal.html", "wget https://snap.stanford.edu/data/com-Youtube.html", and "https://snap.stanford.edu/data/soc-pokec.html"

4. After getting the networks from step3, unzip all the networks.  

