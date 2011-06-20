/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <tulip/ForEach.h>
#include "ObservableGraphTest.h"
#include <tulip/BooleanProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/IntegerProperty.h>

using namespace std;
using namespace tlp;

#include <cppunit/extensions/HelperMacros.h>
CPPUNIT_TEST_SUITE_REGISTRATION( ObservableGraphTest );

// these classes will capture
// everything that will happen to our properties
// synchronously or asynchronously
class ObserverGTest :public Observable {
public:
  std::set<Observable*> observables;
  bool print;

  ObserverGTest() :print(false) {}

  void reset() {
    observables.clear();
  }

  unsigned int nbObservables() {
    return observables.size();
  }

  bool found(Observable *obs) {
    return observables.find(obs) != observables.end();
  }

  // Observable methods
  void update(std::set<Observable *>::iterator begin,
	      std::set<Observable *>::iterator end) {
    //reset();
    std::set<Observable *>::iterator it = begin;
    while(it != end) {
      if (print)
	cout << "inserting in observer..." << endl;
      observables.insert((*it));
      ++it;
    }
  }

  void observableDestroyed(Observable *obs) {
    observables.insert(obs);
  }    
};

static ObserverGTest* observer;

class GraphObserverTest :public GraphObserver, public Observable {
public:

  vector<Graph*> graphs;
  vector<Graph*> sGraphs;
  vector<node> nodes;
  vector<edge> edges;
  string pName;
  string spName;

  Graph* getObservedGraph() {
    assert(graphs.size() == 1);
    return graphs[0];
  }

  vector<Graph*>& getObservedGraphs() {
    return graphs;
  }

  Graph* getObservedSubgraph() {
    assert(sGraphs.size() == 1);
    return sGraphs[0];
  }

  vector<Graph*>& getObservedSubgraphs() {
    return sGraphs;
  }

  node getObservedNode() {
    assert(nodes.size() == 1);
    return nodes[0];
  }
  
  vector<node>& getObservedNodes() {
    return nodes;
  }

  edge getObservedEdge() {
    assert(edges.size() == 1);
    return edges[0];
  }

  vector<edge>& getObservedEdges() {
    return edges;
  }
  
  string& getLocalPropertyName() { return pName; }
  string& getInheritedPropertyName() { return spName; }

  void reset() {
    graphs.clear(), sGraphs.clear();
    nodes.clear(), edges.clear(), pName.clear(), spName.clear();
  }

  void addNode(Graph* g, const node n) {
    graphs.push_back(g), nodes.push_back(n);
  }
  void addEdge(Graph* g, edge e) {
    graphs.push_back(g), edges.push_back(e);
  }
  void delNode(Graph* g, const node n) {
    graphs.push_back(g), nodes.push_back(n);
  }
  void delEdge(Graph* g, edge e) {
    graphs.push_back(g), edges.push_back(e);
  }
  void reverseEdge(Graph* g, edge e) {
    graphs.push_back(g), edges.push_back(e);
  }
  void destroy(Graph* g) {
    graphs.push_back(g);
  }
  void addSubGraph(Graph* g, Graph* sg) {
    graphs.push_back(g), sGraphs.push_back(sg);
  }
  void delSubGraph(Graph* g, Graph* sg) {
    graphs.push_back(g), sGraphs.push_back(sg);
  }
  void addLocalProperty(Graph* g, const string& name) {
    graphs.push_back(g), pName = name;
  }
  void delLocalProperty(Graph* g, const string& name) {
    graphs.push_back(g), pName = name;
  }
  void addInheritedProperty(Graph* g, const string& name) {
    sGraphs.push_back(g), spName = name;
  }
  void afterDelInheritedProperty(Graph* g, const string& name) {
    sGraphs.push_back(g), spName = name;
  }
  virtual void treatEvent(const Event& evt) {
    const GraphEvent* gEvt = dynamic_cast<const GraphEvent*>(&evt);
    if (gEvt) {
      Graph* graph = gEvt->getGraph();
      switch (gEvt->getType()) {
      case GraphEvent::TLP_ADD_INHERITED_PROPERTY:
	addInheritedProperty(graph, gEvt->getPropertyName());
	return;
      case GraphEvent::TLP_AFTER_DEL_INHERITED_PROPERTY:
        afterDelInheritedProperty(graph, gEvt->getPropertyName());
	return;
      default:
	break;
      }
    }
    GraphObserver::treatEvent(evt);
  }
};  

class DelInheritedPropertyObserverTest :public GraphObserver, public Observable {
public:
    DelInheritedPropertyObserverTest():inheritedPropertyExist(false),initialized(false){

    }
    bool inheritedPropertyExist;
    bool initialized;
    void delInheritedProperty(Graph* g, const string& name) {
        if(!initialized){
            inheritedPropertyExist = g->getProperty(name)!=NULL;
            if(inheritedPropertyExist){
                g->getProperty(name)->getName();
            }
            initialized = true;
        }else{
            inheritedPropertyExist = inheritedPropertyExist & (g->getProperty(name)!=NULL);
            if(inheritedPropertyExist){
                g->getProperty(name)->getName();
            }
        }
    }
    virtual void treatEvent(const Event& evt) {
      const GraphEvent* gEvt = dynamic_cast<const GraphEvent*>(&evt);
      if (gEvt) {
        Graph* graph = gEvt->getGraph();
        if (gEvt->getType() == GraphEvent::TLP_AFTER_DEL_INHERITED_PROPERTY)
	  delInheritedProperty(graph, gEvt->getPropertyName());
      }
    }
};

static GraphObserverTest* gObserver;

//==========================================================
void ObservableGraphTest::setUp() {
  graph = tlp::newGraph();
  gObserver = new GraphObserverTest();
  observer = new ObserverGTest();
  graph->addGraphObserver(gObserver);
  graph->addObserver(observer);
}
//==========================================================
void ObservableGraphTest::tearDown() {
  delete graph;
  delete gObserver;
  delete observer;
}
//==========================================================
void ObservableGraphTest::testAddDel() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_NODES = 100;

  for (unsigned int i=0; i<NB_NODES; ++i) {
    gObserver->reset();
    observer->reset();
    nodes.push_back(graph->addNode());
    CPPUNIT_ASSERT(nodes[i] == gObserver->getObservedNode());
    CPPUNIT_ASSERT(graph == gObserver->getObservedGraph());
    CPPUNIT_ASSERT(observer->nbObservables() == 1);
    CPPUNIT_ASSERT(observer->found(graph));
  }

  for (unsigned int i=0; i< NB_NODES; ++i) {
    gObserver->reset();
    observer->reset();
    edges.push_back(graph->addEdge(nodes[i],
				   (i == NB_NODES - 1)
				   ? nodes[0] : nodes[i]));
    CPPUNIT_ASSERT(edges[i] == gObserver->getObservedEdge());
    CPPUNIT_ASSERT(graph == gObserver->getObservedGraph());
    CPPUNIT_ASSERT(observer->nbObservables() == 1);
    CPPUNIT_ASSERT(observer->found(graph));
  }

  for (unsigned int i=0; i < NB_NODES / 2; ++i) {
    gObserver->reset();
    observer->reset();
    graph->delEdge(edges[2 * i]);
    CPPUNIT_ASSERT(edges[2 * i] == gObserver->getObservedEdge());
    CPPUNIT_ASSERT(graph == gObserver->getObservedGraph());
    CPPUNIT_ASSERT(observer->nbObservables() == 1);
    CPPUNIT_ASSERT(observer->found(graph));
  }

  for (unsigned int i=0; i < NB_NODES; ++i) {
    gObserver->reset();
    observer->reset();
    graph->delNode(nodes[i]);
    CPPUNIT_ASSERT(nodes[i] == gObserver->getObservedNode());
    if (i == 0 || (i%2 && i != (NB_NODES - 1))) {
      vector<Graph*>& graphs = gObserver->getObservedGraphs();
      CPPUNIT_ASSERT(graphs.size() == 2);
      CPPUNIT_ASSERT(graphs[0] == graph);
      CPPUNIT_ASSERT(graphs[1] == graph);
      if (i == 0)
	CPPUNIT_ASSERT(edges[NB_NODES - 1] == gObserver->getObservedEdge());
      else
	CPPUNIT_ASSERT(edges[i] == gObserver->getObservedEdge());
    } else {
      CPPUNIT_ASSERT(graph == gObserver->getObservedGraph());
    }
    CPPUNIT_ASSERT(observer->nbObservables() == 1);
    CPPUNIT_ASSERT(observer->found(graph));
 }
}
//==========================================================
void ObservableGraphTest::testClear() {
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_NODES = 100;

  for (unsigned int i=0; i<NB_NODES; ++i) {
    nodes.push_back(graph->addNode());
  }

  for (unsigned int i=0; i< NB_NODES; ++i) {
    edges.push_back(graph->addEdge(nodes[i],
				   (i == NB_NODES - 1)
				   ? nodes[0] : nodes[i]));
  }

  gObserver->reset();
  observer->reset();

  graph->clear();

  vector<node>& oNodes = gObserver->getObservedNodes();
  CPPUNIT_ASSERT(oNodes.size() == NB_NODES);
  for(unsigned int i = 0; i < NB_NODES; ++i) {
    CPPUNIT_ASSERT(nodes[i] == oNodes[i]);
  }
  vector<edge>& oEdges = gObserver->getObservedEdges();
  CPPUNIT_ASSERT(oEdges.size() == NB_NODES);
  for(unsigned int i = 0; i < NB_NODES; ++i) {
    if (i == 0)
      CPPUNIT_ASSERT(oEdges[i] == edges[NB_NODES - 1]);
    else
      CPPUNIT_ASSERT(oEdges[i] == edges[i - 1]);
  }
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(graph));
}
//==========================================================
void ObservableGraphTest::testReverse() {
  vector<node> nodes;
  vector<edge> edges;
  unsigned int NB_NODES = 100;

  for (unsigned int i=0; i<NB_NODES; ++i) {
    nodes.push_back(graph->addNode());
  }

  for (unsigned int i=0; i< NB_NODES; ++i) {
    edges.push_back(graph->addEdge(nodes[i],
				   (i == NB_NODES - 1)
				   ? nodes[0] : nodes[i + 1]));
  }
  for (unsigned int i=0; i< NB_NODES; ++i) {
    gObserver->reset();
    observer->reset();
    graph->reverse(edges[i]);
    CPPUNIT_ASSERT(edges[i] == gObserver->getObservedEdge());
    CPPUNIT_ASSERT(graph == gObserver->getObservedGraph());
    CPPUNIT_ASSERT(observer->nbObservables() == 1);
    CPPUNIT_ASSERT(observer->found(graph));
  }
  // sub graph test
  Graph* g1 = graph->addSubGraph();
  g1->addNode(nodes[0]);
  g1->addNode(nodes[1]);
  g1->addEdge(edges[0]);
  gObserver->reset();
  observer->reset();
  g1->addGraphObserver(gObserver);
  g1->addObserver(observer);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(gObserver->getObservedGraphs().size() == 0);
  g1->reverse(edges[0]);
  // 2 calls only to reverseEdge & update
  CPPUNIT_ASSERT(gObserver->getObservedEdges().size() == 2);
  CPPUNIT_ASSERT(gObserver->getObservedGraphs().size() == 2);
  CPPUNIT_ASSERT(observer->nbObservables() == 2);
}
//==========================================================
void ObservableGraphTest::testSubgraph() {
  Graph *g1, *g2, *g3, *g4;

  gObserver->reset();
  observer->reset();
  g1 = graph->addSubGraph();
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g1);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(graph));
  gObserver->reset();
  observer->reset();
  g2 = graph->addSubGraph();
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g2);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(graph));
  gObserver->reset();
  observer->reset();
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  g2->addGraphObserver(gObserver);
  g2->addObserver(observer);
  g3 = g2->addSubGraph();
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g3);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));
  gObserver->reset();
  observer->reset();
  g4 = g2->addSubGraph();
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g4);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));

  gObserver->reset();
  observer->reset();
  g3->addGraphObserver(gObserver);
  g3->addObserver(observer);
  Observable::holdObservers();
  // 2 hold more for testing purpose
  Observable::holdObservers();
  Observable::holdObservers();
  node n1 = g3->addNode();
  vector<Graph*>& graphs = gObserver->getObservedGraphs();
  CPPUNIT_ASSERT(graphs.size() == 3);
  CPPUNIT_ASSERT(graphs[0] == graph);
  CPPUNIT_ASSERT(graphs[1] == g2);
  CPPUNIT_ASSERT(graphs[2] == g3);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(Observable::observersHoldCounter() == 3);
  // first unhold
  Observable::unholdObservers();
  // nothing happens
  CPPUNIT_ASSERT(Observable::observersHoldCounter() == 2);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(observer->found(graph) == false);
  CPPUNIT_ASSERT(observer->found(g2) == false);
  CPPUNIT_ASSERT(observer->found(g3) == false);
  // second unhold
  Observable::unholdObservers();
  CPPUNIT_ASSERT(Observable::observersHoldCounter() == 1);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(observer->found(graph) == false);
  CPPUNIT_ASSERT(observer->found(g2) == false);
  CPPUNIT_ASSERT(observer->found(g3) == false);
  // third unhold
  Observable::unholdObservers();
  CPPUNIT_ASSERT(Observable::observersHoldCounter() == 0);
  CPPUNIT_ASSERT(observer->nbObservables() == 3);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2));
  CPPUNIT_ASSERT(observer->found(g3));
  vector<node>& nodes = gObserver->getObservedNodes();
  CPPUNIT_ASSERT(nodes.size() == 3);
  CPPUNIT_ASSERT(nodes[0] == n1);
  CPPUNIT_ASSERT(nodes[1] == n1);
  CPPUNIT_ASSERT(nodes[2] == n1);
  
  gObserver->reset();
  observer->reset();
  g4->addGraphObserver(gObserver);
  g4->addObserver(observer);
  node n2 = g4->addNode();
  CPPUNIT_ASSERT(graphs.size() == 3);
  CPPUNIT_ASSERT(graphs[0] == graph);
  CPPUNIT_ASSERT(graphs[1] == g2);
  CPPUNIT_ASSERT(graphs[2] == g4);
  CPPUNIT_ASSERT(observer->nbObservables() == 3);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2));
  CPPUNIT_ASSERT(observer->found(g4));
  CPPUNIT_ASSERT(nodes.size() == 3);
  CPPUNIT_ASSERT(nodes[0] == n2);
  CPPUNIT_ASSERT(nodes[1] == n2);
  CPPUNIT_ASSERT(nodes[2] == n2);
  
  gObserver->reset();
  observer->reset();
  Observable::holdObservers();
  edge e = g2->addEdge(n1, n2);
  CPPUNIT_ASSERT(graphs.size() == 2);
  CPPUNIT_ASSERT(graphs[0] == graph);
  CPPUNIT_ASSERT(graphs[1] == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 2);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2));
  vector<edge>& edges = gObserver->getObservedEdges();
  CPPUNIT_ASSERT(edges.size() == 2);
  CPPUNIT_ASSERT(edges[0] == e);
  CPPUNIT_ASSERT(edges[1] == e);
  
  gObserver->reset();
  observer->reset();
  Observable::holdObservers();
  g2->delNode(n2);
  CPPUNIT_ASSERT(graphs.size() == 3);
  CPPUNIT_ASSERT(graphs[0] == g2);
  CPPUNIT_ASSERT(graphs[1] == g4);
  CPPUNIT_ASSERT(graphs[2] == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 2);
  CPPUNIT_ASSERT(observer->found(g2));
  CPPUNIT_ASSERT(observer->found(g4));
  CPPUNIT_ASSERT(nodes.size() == 2);
  CPPUNIT_ASSERT(nodes[0] == n2);
  CPPUNIT_ASSERT(nodes[1] == n2);  
  CPPUNIT_ASSERT(edges.size() == 1);
  CPPUNIT_ASSERT(edges[0] == e);

  gObserver->reset();
  observer->reset();
  g2->addNode(n2);
  CPPUNIT_ASSERT(graphs[0] == gObserver->getObservedGraph());
  CPPUNIT_ASSERT(n2 == gObserver->getObservedNode());
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));
  gObserver->reset();
  observer->reset();
  g2->addEdge(e);
  CPPUNIT_ASSERT(graphs[0] == gObserver->getObservedGraph());
  CPPUNIT_ASSERT(e == gObserver->getObservedEdge());
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));

  gObserver->reset();
  observer->reset();
  Observable::holdObservers();
  g2->delNode(n2, true);
  CPPUNIT_ASSERT(graphs.size() == 4);
  CPPUNIT_ASSERT(graphs[0] == graph);
  CPPUNIT_ASSERT(graphs[1] == g2);
  CPPUNIT_ASSERT(graphs[2] == g2);
  CPPUNIT_ASSERT(graphs[3] == graph);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 2);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2));
  CPPUNIT_ASSERT(nodes.size() == 2);
  CPPUNIT_ASSERT(nodes[0] == n2);
  CPPUNIT_ASSERT(nodes[1] == n2);  
  CPPUNIT_ASSERT(edges.size() == 2);
  CPPUNIT_ASSERT(edges[0] == e);
  CPPUNIT_ASSERT(edges[1] == e);
}
//==========================================================
void ObservableGraphTest::testDeleteSubgraph() {
  Graph *g1, *g2, *g3, *g4;

  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g2->addGraphObserver(gObserver);
  g2->addObserver(observer);
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  gObserver->reset();
  observer->reset();
  g2->delSubGraph(g3);
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g3);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));

  gObserver->reset();
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::holdObservers();
  g2->delSubGraph(g4);
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g4);
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));

  gObserver->reset();
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  graph->delAllSubGraphs(g2);
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g2);
  vector<Graph*>& graphs = gObserver->getObservedGraphs();
  CPPUNIT_ASSERT(graphs[0] == graph);
  CPPUNIT_ASSERT(graphs[1] == g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 2);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2));

  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::holdObservers();
  g2 = graph->addSubGraph();
  g2->addObserver(observer);
  g3 = g2->addSubGraph();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  g3->addObserver(observer);
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  g2->delSubGraph(g3);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g3));
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  graph->delSubGraph(g2);
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(g2));
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 1);
  CPPUNIT_ASSERT(observer->found(graph));
  CPPUNIT_ASSERT(observer->found(g2) == false);
}
//==========================================================
void ObservableGraphTest::testAddDelProperties() {
  Graph *g1, *g2;

  CPPUNIT_ASSERT(gObserver->graphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "");
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");

  BooleanProperty* bp = graph->getProperty<BooleanProperty>("test");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");

  gObserver->reset();
  CPPUNIT_ASSERT(gObserver->graphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");
  graph->delLocalProperty("test");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");
  
  g1 = graph->addSubGraph();
  g1->addGraphObserver(gObserver);
  gObserver->reset();

  bp = graph->getProperty<BooleanProperty>("test");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test");
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g1);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "test");

  gObserver->reset();
  CPPUNIT_ASSERT(gObserver->graphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");

  graph->delLocalProperty("test");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == graph);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test");
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g1);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "test");

  g2 = g1->addSubGraph();
  g2->addGraphObserver(gObserver);
  gObserver->reset();
  CPPUNIT_ASSERT(gObserver->graphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");

  bp = g1->getProperty<BooleanProperty>("test1");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g1);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test1");
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g2);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "test1");

  gObserver->reset();
  CPPUNIT_ASSERT(gObserver->graphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "");
  CPPUNIT_ASSERT(gObserver->sGraphs.size() == 0);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "");

  g1->delLocalProperty("test1");
  CPPUNIT_ASSERT(gObserver->getObservedGraph() == g1);
  CPPUNIT_ASSERT(gObserver->getLocalPropertyName() == "test1");
  CPPUNIT_ASSERT(gObserver->getObservedSubgraph() == g2);
  CPPUNIT_ASSERT(gObserver->getInheritedPropertyName() == "test1");
}
//==========================================================
void ObservableGraphTest::testObserverWhenRemoveObservable() {
  CPPUNIT_ASSERT(graph->countGraphObservers() == 1);
  GraphObserverTest *graphObserverTmp=new GraphObserverTest();
  graph->addGraphObserver(graphObserverTmp);
  CPPUNIT_ASSERT(graph->countGraphObservers() == 2);
  delete graphObserverTmp;
  CPPUNIT_ASSERT(graph->countGraphObservers() == 1);
  
  /*CPPUNIT_ASSERT(graph->countObservers() == 1);
  ObserverGTest *observerTmp=new ObserverGTest();
  graph->addObserver(observerTmp);
  CPPUNIT_ASSERT(graph->countObservers() == 2);
  delete observerTmp;
  CPPUNIT_ASSERT(graph->countObservers() == 1);*/
}

void ObservableGraphTest::testDelInheritedPropertyExistWhenDelInheritedPropertyIsSend() {
    Graph *g1 = graph->addSubGraph();
    DelInheritedPropertyObserverTest *observer = new DelInheritedPropertyObserverTest();
    g1->addGraphObserver(observer);
    graph->getLocalProperty<BooleanProperty>("test");
    graph->delLocalProperty("test");
    CPPUNIT_ASSERT(observer->initialized);
    CPPUNIT_ASSERT(observer->inheritedPropertyExist);
}

//==========================================================
CppUnit::Test * ObservableGraphTest::suite() {
  CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Tulip lib : Graph" );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Add/Del operations (Simple Graph)", 
								  &ObservableGraphTest::testAddDel) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Clean operations (Simple Graph)", 
								  &ObservableGraphTest::testClear) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Edge reversing", 
								  &ObservableGraphTest::testReverse) );

  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Sub Graph operations (add/del/clean/iterators)", 
								  &ObservableGraphTest::testSubgraph) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Test Sub Graph delete", 
								  &ObservableGraphTest::testDeleteSubgraph) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Add/Del  properties operations (Graph & subgraphs)", 
								  &ObservableGraphTest::testAddDelProperties) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Test Observer when remove Observable", 
								  &ObservableGraphTest::testObserverWhenRemoveObservable) );

  suiteOfTests->addTest( new CppUnit::TestCaller<ObservableGraphTest>( "Test if the property exist when delInherited function is called",
                                                                  &ObservableGraphTest::testDelInheritedPropertyExistWhenDelInheritedPropertyIsSend) );


  return suiteOfTests;
}
//==========================================================
