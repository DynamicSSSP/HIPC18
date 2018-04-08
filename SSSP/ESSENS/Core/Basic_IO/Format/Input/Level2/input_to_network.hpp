#ifndef INPUT_TO_NETWORK_HPP
#define INPUT_TO_NETWORK_HPP

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "ADJ/create_network.hpp"
#include "CSR/create_network.hpp"
using namespace std;

/****List of Functions ******/
//==Level2 Functions	
/*readin_Network
 ==Default buffer size=0
 ==Default input_file "E_input.txt"
 == Calls
 create_network()
 --Overloaded as;
 (Network); DONE
 (Network, buffer); DONE
 (input_file,Network);
 (input_file,Network,buffer);*/
 
 
/**** Reading File to Create Network******/
template <class myNetworkType>
void readin_network(myNetworkType *X, char *file, int XtraN)
{
	//Activity0: Reads in File to a vector of edges <edgelist>
	
	//File reading parameters
	FILE *graph_file;
	char line[128];
    int nodes;
    
	vector<Edge> a ;//list of all edges of network
	a.resize(0);
	Edge myedge;
	 
	graph_file=fopen(file, "r");
	while(fgets(line,128,graph_file) != NULL)
	{
        int n1,n2;
		//Read line 
		sscanf(line,"%d %d %lf",&n1,&n2,&myedge.edge_wt);
        myedge.node1=n1;
        myedge.node2=n2;
        a.push_back(myedge);
        
	}//end of while
	fclose(graph_file);


	//Activity1: Creates specified network format
	create_Network(&a, 0, X,XtraN);
	
	return;
}

/********* End of Function **********/


/**** Reading File to Create Network******/
//These are for directed networks which can start with 0 or 1
template <class myNetworkType>
void readin_network(myNetworkType *X, char *file, int fst, int XtraN)
{
    //Activity0: Reads in File to a vector of edges <edgelist>
    
    //File reading parameters
    FILE *graph_file;
    char line[128];
    int nodes;
    
    vector<Edge> a ;//list of all edges of network
    a.resize(0);
    Edge myedge;
    
    graph_file=fopen(file, "r");
    while(fgets(line,128,graph_file) != NULL)
    {
        int n1,n2;
        //Read line
        sscanf(line,"%d %d %lf",&n1,&n2,&myedge.edge_wt);
        myedge.node1=n1-fst;
        myedge.node2=n2-fst;
        a.push_back(myedge);
        
        myedge.node1=n2-fst;
        myedge.node2=n1-fst;
        a.push_back(myedge);
        
        
    }//end of while
    fclose(graph_file);
    sort(&a, inc_node1);
    a=unique(a);
    
    cout <<":::"<< a.size() <<"\n";
    
    //Activity1: Creates specified network format
    create_Network(&a, 0, X,XtraN);
    
    return;
}

/********* End of Function **********/

/**** I/O error handler function ***/
void network_IO_handle_error(const char* msg) {
    perror(msg); 
    exit(255);
}

/**** Reading File to Create Network******/
//These are for directed networks which can start with 0 or 1
template <class myNetworkType>
void readin_network_posix(myNetworkType *X, char *file, int fst, int XtraN)
{
    //Activity0: Reads in File to a vector of edges <edgelist>

    //File reading parameters
    char line[128];
    int nodes;
    const size_t BUFFER_SIZE = 16*1024;

    int graph_file = open(file, O_RDONLY);
    if(graph_file == -1)
        network_IO_handle_error("open");

    vector<Edge> a ;//list of all edges of network
    //a.resize(0);

    /* Advise the kernel of our access pattern.  */
    posix_fadvise(graph_file, 0, 0, 1);  // FDADVICE_SEQUENTIAL

    char buf[BUFFER_SIZE + 1];
    while(size_t bytes_read = read(graph_file, buf, BUFFER_SIZE))
    {
        if(bytes_read == (size_t)-1)
            network_IO_handle_error("read failed");
        if (!bytes_read)
            break;

        for (char *p = buf; (p = (char*) memchr(p, '\n', (buf + bytes_read) - p)); ++p) {
            int n1,n2;
            Edge myedge;
            //Read line
            sscanf(p,"%d %d %lf",&n1,&n2,&myedge.edge_wt);
            //cout << "reading " << n1 << " " << n2 << " " << myedge.edge_wt << endl;
            myedge.node1=n1-fst;
            myedge.node2=n2-fst;
            a.push_back(myedge);
    
            myedge.node1=n2-fst;
            myedge.node2=n1-fst;
            a.push_back(myedge);
        } // end for

    }//end of while

    close(graph_file);
    sort(&a, inc_node1);
    a=unique(a);

    cout <<":::"<< a.size() <<"\n";

    //Activity1: Creates specified network format
    create_Network(&a, 0, X,XtraN);

    return;
}

/********* End of Function **********/

/**** Reading File to EdgeList******/
void readin_networkE(char *myfile, vector<Edge> *EdgeL, int *nodes)
{
    //Activity0: Reads in File to a vector of edges <edgelist>
    
    //File reading parameters
    FILE *graph_file;
    char line[128];
    EdgeL->resize(0);
    Edge myedge;
    *nodes=-1;
    
    graph_file=fopen(myfile, "r");
    while(fgets(line,128,graph_file) != NULL)
    {
        //Read line
        sscanf(line,"%d %d %lf",&myedge.node1,&myedge.node2,&myedge.edge_wt);
        if(myedge.node1>*nodes){*nodes=myedge.node1;}
        if(myedge.node2>*nodes){*nodes=myedge.node2;}
        EdgeL->push_back(myedge);
        
    }//end of while
    *nodes=*nodes+1;
    fclose(graph_file);
    
    sort(EdgeL, inc_node1);
    
    return;
}

/********* End of Function **********/

	
#endif
