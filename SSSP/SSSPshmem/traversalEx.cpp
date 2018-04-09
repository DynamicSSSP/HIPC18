

//INPUT HEADERS
#include "translate_from_input.hpp"
#include "input_to_network.hpp"
#include"structure_defs.hpp"

#include "ADJ/traversal.hpp"

//OUTPUT HEADERS
#include "printout_network.hpp"
#include "printout_others.hpp"

//PERMUTE
#include "permute_elements.hpp"
#include "ADJ/permute_vertex.hpp"
#include "DJK.hpp"

using namespace std;

int main(int argc, char *argv[])
{       
	clock_t q, q1, q2,t;
	
    /***** Preprocessing to Graph (GUI) ***********/
    q=clock();
    //Check if valid input is given
    if ( argc < 3) { cout << "INPUT ERROR:: 2 inputs required. First: filename \n Second: file begins with 1/0  return 0 \n";}
    //Check to see if file opening succeeded
    ifstream the_file ( argv[1] ); if (!the_file.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}
    
    A_Network X;
         /******* Read Graph (GUI) and Create Reverse Map*****************/
        //Obtain the list of edges.
        q=clock();
    int fst=atoi(argv[2]);
         readin_network(&X,argv[1],fst,-1);
        
        q=clock()-q;
        cout << "Total Time for Reading Network"<< ((float)q)/CLOCKS_PER_SEC <<"\n";
        /**** Read Graph (GUI) ***********/
    
    int nodes=X.size();

    A_Network Y=X;

//MAXST Traversal
    A_Network Y1;
    
        djk(0,&Y,&Y1);
        //traversal(0,Y,"bfs",&Y1);
    
    printf("done \n");

//Find the edges that are in Y but not in Y1
    ADJ_Bundle AList;
    A_Network Z;
    Z.clear();
    for(int i=0; i<Y.size();i++)
    {
        AList.ListW.clear();
        AList.Row=i;
        AList.ListW=difference(Y[i].ListW, Y1[i].ListW);
        Z.push_back(AList);
        
        //printf("%d --\n", i);
    }
    //cout <<Z.size() << "\n";
  print_network(Y1, "GraphC.txt");
  print_network(Y,"Graphall.txt");
  print_network(Z, "Graphdiff.txt");
  
    
   
	return 0;
}//end of main
	
	//==========================================//

