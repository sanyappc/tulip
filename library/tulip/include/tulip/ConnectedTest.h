//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 01/09/2003
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef TULIP_CONNECTEDTEST_H
#define TULIP_CONNECTEDTEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if (__GNUC__ < 3)
#include <hash_set>
#else
#include <ext/hash_set>
#endif
#include "tulip/ObservableGraph.h"

namespace tlp {

class Graph;

/** \addtogroup graph_test */ 
/*@{*/
/// class for testing if the graph is connected
class TLP_SCOPE ConnectedTest : private GraphObserver {
public:
  /**
   * Return true if the graph is connected (ie. it exists an undirected path 
   * between each pair of nodes) else false.
   */
  static bool isConnected(Graph *graph);
  /**
   * If the graph is not connected, add edges in order to make the graph
   * connected. The new edges are added in addedEdges.
   */
  static void makeConnected(Graph *graph, std::vector<edge>& addedEdges);
  /**
   * Return the number of connected componnents in the graph;
   */
  static unsigned int numberOfConnectedComponnents(Graph *graph);

private:
  void connect(Graph *, std::vector<node>& toLink);
  bool compute(Graph *);
  void addEdge(Graph *,const edge);
  void delEdge(Graph *,const edge);
  void reverseEdge(Graph *,const edge);
  void addNode(Graph *,const node);
  void delNode(Graph *,const node);
  void destroy(Graph *);
  ConnectedTest();
  static ConnectedTest * instance;
  stdext::hash_map<unsigned int,bool> resultsBuffer;
};

}
/*@}*/
#endif
