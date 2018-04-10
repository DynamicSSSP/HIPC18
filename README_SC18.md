# SC18 Paper Reproducibility Instructions

## Getting Started
First, follow the installation instructions in the [README](README.md)

For the analysis scripts, you will need a Python (2 or 3) installation that also has the Seaborn module (for plotting).

### Datasets for the experiments

Following are the steps to setup RMAT network and real network for running Dynamic SSSP and Galois

1. Compile RMAT graph generator (in external/RMAT) by issuing "make" command (without quotes).

2. Generate RMAT networks for scale 24 for different probablities, by issuing following commands 
"./driverForRmat 24 24 0.25 0.25 0.25 0.25 data/RMAT24.er"
"./driverForRmat 24 24 0.45 0.15 0.15 0.25 data/RMAT24.gr"

3. For real world networks (https://snap.stanford.edu/data/), following command were used from unix machine-
"wget https://snap.stanford.edu/data/com-LiveJournal.html", "wget https://snap.stanford.edu/data/com-Youtube.html", and "https://snap.stanford.edu/data/soc-pokec.html"

4. After getting the networks from step3, unzip all the networks.  
6. To complie the code, set the path to ESSENS directory in the Makefile. Code will not compile if files in ESSSENS are not linked

7. Creating the inputs: Our code takes as input (i) the original SSSP Tree, (ii) the remainder edges in the graph (not in the SSSP tree) and (iii) the set of changed edges.

7a Creating input (i) and (ii)
    make tEx (compile traversalEx.cpp)
execute: 

Source is set at line 47: int src=0

./tEx.out <filename> <starts with 1 or 0>
Example: ./tEx.out RMAT18ER 1
The output will be four files
Graphall.txt (Original file, starting from 0, undirected)
GraphC.txt  (the edgelist for the SSSP tree)
GraphT.txt  ( the SSSP tree with parent information. The columns are: node id , parent, edge weight between node and parent, distance from the source)
Graphdiff.txt  (remaining edges, not in certificate)

7b  Creating input (iii)
max cE (compile create_edgelist.cpp) 
execute:
./cE.out Graphall.txt <number of edges> <maximum weight of new edges> <Percentage of inserted edges (0 to 100) > changed_edges
Each line in the changed_edges file consists of 4 tuples
n1 n2 edge_wt 1/0  (1:insertion 0 is deletion)

Example: ./cE.out Graphall.txt 500 100 80 > changedRMAT18_500
The output changedRMAT18_500 will contain 500 edges of which 80% and insertions (marked with 1) and others are deletions (marked with 0). The edge weights are capped at 100.

8. Run the parallel dynamic code (Main Contribution)

make all (compile main_code1x.cpp)

The source is set to 0 in line 62 main_code1x.cpp. 
Make sure that the source in the  traversalEx.cpp (original traversal) matches the src in main_code1x.cpp
int src=0; //the source from which the paths are computed


SEVEN inputs required. 
First: filename without tree(Graphdiff.txt). 
Second: SSSP as a Graph (GraphC.txt) 
Third: SSSP as a tree (GraphT.txt)
Fourth: List of Edges to be Updated (changed_edges)
Fifth: Upper Bound on Weight (100)
Sixth: number of nodes in graph 
Seventh: number of threads

Example:./a.out Graphdiff.txt GraphC.txt GraphT.txt changed_edges 100 262144 4

The output is:

Total Time for Initial Update (This is time for Step 1)
Total Time for Complete Update (This is time for Step 2)
printRT(&SSSP)  prints the list of vertices along with their parents and distance form the source.  This is currently commented out becuase the networks are too large


