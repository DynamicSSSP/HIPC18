
// HEADERS
#include "SP_structure.hpp"


using namespace std;

/*** All Headers Required From ESSENS **/


int main(int argc, char *argv[])
{       
	
	
	/***** Preprocessing to Graph (GUI) ***********/

    //Assume Processed Input
    //Form node node weight
    //Nodes are numbered contiguously
    //Starts from zero
    //Edges are undirected (both a-->b and b-->a present)
	//Check if valid input is given
	if ( argc < 2 ) { cout << "INPUT ERROR:: Three inputs required. First: filename. Second: number of changed edges. Third: Maximum Weight of new edges. Fourth: Percentage of Inserted Edges (in values 0 to 100) \n" ; return 0;}
	//Check to see if file opening succeeded
	ifstream the_file ( argv[1] ); if (!the_file.is_open() ) { cout<<"INPUT ERROR:: Could not open main file\n";}
/*** Create DataStructure Sparsifictaion Tree **/
    
    
    /******* Read Graph to EdgeList****************/
    vector<Edge> allEdges;
    allEdges.clear();
    int nodes;
    readin_networkE(argv[1],&allEdges, &nodes);
    nodes+1;
    /******* Read Graph to EdgeList****************/
    
    /**** Create Set of Edges to Modify ****/
    srand (time(NULL));
    int i=0;
    int numE=atoi(argv[2]);
    int max_wt= atoi(argv[3]);
    int ins_per=atoi(argv[4]);
    while(i<numE)
    {
        int nx=rand()%nodes;
        int ny=rand()%nodes;
        
        int k=rand()%100;
        
        Edge mye;
        
        //Create an Edge to Delete
        if(k>ins_per)
        {   
            int nz=rand()%(allEdges.size());
            mye=allEdges[nz];
            printf("%d  %d  %f 0 \n", mye.node1, mye.node2, mye.edge_wt);
            i++;
            continue;
        }
        
        if(nx==ny){continue;}
        i++;
        
        int n1, n2;
        if(nx<ny) {n1=nx; n2=ny;}
        else
        {n1=ny; n2=nx;}
        
        bool found=false;
        for(int j=0; j<allEdges.size(); j++)
        {  mye=allEdges[j];
          if(mye.node1==n1 && mye.node2==n2) //Edge found==to be deleted
          {
              printf("%d  %d %f 0\n", mye.node1, mye.node2,mye.edge_wt);
              found=true;
              break;}
            if(mye.node1 > n1) {break;}
        }
    
            if(!found) //Edge to be inserted
            {
                double mywt=rand()%max_wt;
             printf("%d  %d %f 1 \n", n1, n2, mywt);
            }
    
    }
return 0;	
}//end of main
	
	//==========================================//
	
	

