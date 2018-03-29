/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Mahantesh Halappanavar                           */
/*                        High Performance Computing                         */
/*                Pacific Northwest National Lab, Richland, WA               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Copyright (C) 2010 Mahantesh Halappanavar                                 */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or (at your option) any later version.                    */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place-Suite 330,Boston,MA 02111-1307,USA.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#include "defs.h"

void writeSimpleGraphFormatNewD(graph* G, char * filename, int weighted)
{
  //Get the iterators for the graph:
  long NVer     = G->numVertices;
  long NS       = G->sVertices;
  long NT       = NVer - NS;
  long NEdge    = G->numEdges;       //Returns the correct number of edges (not twice)
  long *verPtr  = G->edgeListPtrs;   //Vertex Pointer: pointers to endV
  edge *verInd = G->edgeList;       //Vertex Index: destination id of an edge (src -> dest)
  printf("NVer= %ld --  NE=%ld\n", NVer, NEdge);

  printf("Writing graph in simple format - Undirected graph - each edge represented ONLY ONCE\n");
  printf("1-based indices will be used.");
  printf("There can be isolated vertices.\n");
  printf("Graph will be stored in file %s...\n",filename);
  
  FILE *fout;
  fout = fopen(filename, "w");
  if (!fout) {
    printf("Could not open the file \n");
    exit(1);
  }
  //First Line:
  //fprintf(fout, "%ld %ld U\n", NVer, NEdge);
  
  for (long v=0; v<NVer; v++) {
    long adj1 = verPtr[v];
    long adj2 = verPtr[v+1];
    //Edge lines: <adjacent> <weight>
    for(long k = adj1; k < adj2; k++ ) {
      if(verInd[k].tail > v)
        if (weighted)
          fprintf(fout, "%ld %ld %g\n", v+1, verInd[k].tail+1, verInd[k].weight );
        else
          fprintf(fout, "%ld %ld\n", v+1, verInd[k].tail+1);
    } //End of for loop to traverse adjacency of v
  }
  fclose(fout);
  printf("Graph has been stored in file: %s\n",filename);
  
}//End of writeGraphDimacsOneFormatNewD()
