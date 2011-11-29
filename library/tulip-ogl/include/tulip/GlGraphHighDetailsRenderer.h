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
#ifndef Tulip_GLGRAPHHIGHDETAILSRENDERER_H
#define Tulip_GLGRAPHHIGHDETAILSRENDERER_H

#include <tulip/GlGraphRenderer.h>

#include <vector>

#include <tulip/Observable.h>

#include <tulip/GlGraphRenderingParameters.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/GlNode.h>
#include <tulip/GlEdge.h>
#include <tulip/GlQuadTreeLODCalculator.h>
#include <tulip/GlScene.h>

namespace tlp {

class Graph;

class TLP_GL_SCOPE GlGraphHighDetailsRenderer : public GlGraphRenderer {

public:

  /**
   * Build a GlGraphComposite with the graph data
   */
  GlGraphHighDetailsRenderer(Graph* graph,GlGraphInputData &inputData,GlGraphRenderingParameters &parameters);

  ~GlGraphHighDetailsRenderer();

  virtual void draw(float lod,Camera* camera);

  void initSelectionRendering(RenderingEntitiesFlag type,std::map<unsigned int, SelectedEntity> &idMap,unsigned int &currentId);

  void visitGraph(GlSceneVisitor *visitor,bool visitHiddenEntities=false);

protected:

  void buildSortedList();
  void visitNodes(Graph *graph,GlSceneVisitor *visitor,bool visitHiddenEntities=false);
  void visitEdges(Graph *graph,GlSceneVisitor *visitor,bool visitHiddenEntities=false);

  void drawLabelsForComplexEntities(bool drawSelected,OcclusionTest *occlusionTest,LayerLODUnit &layerLODUnit);

  GlLODCalculator *lodCalculator;


  GlScene *fakeScene;
};
}

#endif
