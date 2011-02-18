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
#ifndef CLUSTERINGALGORITHMBASE_H
#define CLUSTERINGALGORITHMBASE_H

#include "tulip/Algorithm.h"
#include "tulip/MutableContainer.h"

namespace tlp {
  class DoubleProperty;
  class ClusteringQualityMeasure;

  enum SubGraphsCheck {
    OK = 0,
    MissingNodes = 1,
    DuplicateNodes = 2
  };

  class TLP_SCOPE ClusteringAlgorithmBase : public tlp::Algorithm {
    public:
      ClusteringAlgorithmBase(AlgorithmContext context);
      virtual ~ClusteringAlgorithmBase();
      virtual bool runClustering() = 0;
      virtual tlp::ClusteringQualityMeasure* getQualityMeasure() = 0;
            
      const Graph* getOriginalGraph() const;
      const Graph* getQuotientGraph() const;
      const DataSet& getDataSet() const;
      
      double getIntraEdges(tlp::node n) const;
      double getExtraEdges(tlp::edge e) const;
      double getExtraEdges(tlp::node n) const;
      int getPartitionSize() const;
      int getPartitionSize(int paritionIndex) const;
      int getPartitionSize(tlp::node quotientNode) const;
      unsigned int getPartitionId(tlp::node n) const;
      
      void orderByPartitionId(node &n1, node &n2) const;
    protected:
      virtual void initializeFromSubGraphs() = 0;
      virtual void cannotInitializeFromSubGraphs(const std::string& message) = 0;
      SubGraphsCheck checkSubGraphsForDuplicateOrMissingNodes();
      tlp::Graph* _quotientGraph;
      
      std::vector<std::set<node> > _partition; //partition of nodes
      std::vector<std::vector<std::set<node > > > _hierarchy; //the hierarchy of partitions
      MutableContainer<unsigned int> _partitionId;  //node of the Quotient Graph -> partition id
      MutableContainer<node> _partitionNode;        //partition id -> node in the quotient graph
      
      DoubleProperty* _intraEdges;
      DoubleProperty* _extraEdges;
      
      double bestQuality;
      int best_ind;
      
      DoubleProperty* _metric;
      
      tlp::ClusteringQualityMeasure* _qualityMeasure;
      bool _useSubGraphsAsInitialClustering;
      static std::string useSubGraphs;
    private:
      bool run();
  };
  
  class TLP_SCOPE AgglomerativeClusteringBase : public ClusteringAlgorithmBase {
    public:
      AgglomerativeClusteringBase(tlp::AlgorithmContext);
      virtual bool runClustering() = 0;
      virtual bool merge() = 0;
      
      /**
      * Merges nodes together.
      * This implementation deletes only one of the two nodes to reduce overhead.
      * The node with fewer edges is deleted, and all the edges are redirected to the remaining node.
      */
      tlp::node mergeNodes(tlp::node n1, tlp::node n2);

      void simpleGraphCopy(const tlp::Graph* source, tlp::Graph* target, MutableContainer<node>& nodeMapping);
      void buildHierarchy(tlp::Graph* graph, std::vector<std::vector<std::set<node > > >& partitions, int best_ind = -1);
  };
   
  class TLP_SCOPE DivisiveClusteringBase : public ClusteringAlgorithmBase {
    public:
      DivisiveClusteringBase(tlp::AlgorithmContext context);
      virtual bool runClustering() = 0;
      
      bool splitGraphIfDisconnected(int clusterIndex, tlp::Graph*const cluster, tlp::Graph** subCluster1, tlp::Graph** subCluster2);
      tlp::edge findEdgeToRemove();
    protected:
      int metric_mode;
      std::string _metricAlgorithm;
      Graph* _workingGraph;
      std::map<tlp::node, tlp::node> _originalToQuotient;
      std::vector<Graph*> _currentClusters;
    private:
      double _sumEdges;
  };
}
#endif //CLUSTERINGALGORITHMBASE_H
