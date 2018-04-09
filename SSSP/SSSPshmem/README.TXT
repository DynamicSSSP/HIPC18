
Set the path to ESSENS directory in the Makefile

Compile The Codes
make all
make cE
make tEx

Create RMAT File using generator code

Delete Duplicate Edges
awk '{if($1< $2) print $1" " $2; else print $2" "$1 }' RMAT18_all.txt| sort -n -k1 -k2 |uniq  > tmp
mv tmp RMAT18ER

Set Edge weights from 0 to 100
awk '{print $1 " " $2 " " int(rand()*100)}' RMAT18ER > tmp
mv tmp RMAT18ER

Then Create the SSSP Tree and the Remaining Graph
./tEx.out <filename> <starts with 1 or 0>
Example: ./tEx.out ./DATA/RMAT18ER 1
this will create three files
Graphall.txt (Original file, starting from 0, undirected)
GraphC.txt  (the certificate files, SSSP Tree)
Graphdiff.txt  (remaining edges, not in certificate)

===================================

Create set of changed Edges
./cE.out <filename> <number of edges> <maximum weight of new edges> <Percentage of inserted edges (0 to 100)>
Example: ./cE.out ./DATA/RMAT18ER 500 100 80 > ./DATA/changedRMAT18_500

Sort the edges for faster run—Optional
sort -n -k1 <filename> > output
Example: sort -n -k1 ./DATA/changedRMAT18_500 > ./DATA/changedRMAT18_500S


========================
Run the update
./a.out
SIX inputs required. 
First: filename without tree. 
Second: Graph Certificate. 
Third: List of Edges to be Updated. 
Fourth: Upper Bound on Weight 
Fifth: number of nodes. 
Sixth: number of threads

Example:./a.out ./DATA/RMAT18ER_diff ./DATA/RMAT18ER_C ./DATA/changedRMAT18_500S 100 262144 4


