#ifndef _DEFS_H
#define _DEFS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define MilanRealMax HUGE_VAL       // +INFINITY
#define MilanRealMin -MilanRealMax  // -INFINITY

typedef struct /* the edge data structure */
{
  //long id;
  long head;
  long tail;
  //int weight;
  double weight;
} edge;

typedef struct /* the graph data structure */
{
  long numVertices;        /* Number of columns                                */
  long sVertices;          /* Number of rows: Bipartite graph: number of S vertices; T = N - S */
  long numEdges;           /* Each edge stored twice, but counted once        */
  long * edgeListPtrs;     /* start vertex of edge, sorted, primary key        */
  edge * edgeList;         /* end   vertex of edge, sorted, secondary key      */
} graph;

/////////////////// FUNCTION CALLS ////////////////////
#ifdef __cplusplus
extern "C" {
#endif
  graph * generateRMAT(int SCALE, int SCALE_WT, int epv, double a, double b, double c, double d);
  void displayGraphCharacteristics(graph *G);
  void writeSimpleGraphFormatNewD(graph* G, char * filename, int weighted);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif


#endif
