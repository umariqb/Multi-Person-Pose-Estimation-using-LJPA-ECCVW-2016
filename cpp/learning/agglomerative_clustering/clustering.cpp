/*
 * clustering.cpp
 *
 *  Created on: May 26, 2009
 *      Author: lbossard
 */

#include "clustering.hpp"

#include <glog/logging.h>


namespace awesomeness {
namespace learning {
namespace agglomerative_clustering {

void label_leaves(
    const unsigned int clusterLabel,
    const unsigned int nodeId,
    const std::vector<std::pair<unsigned int, unsigned int> >& clusterTree,
    std::vector<unsigned int>& elementsToClusterMapping) {

  const unsigned int numLeaves = elementsToClusterMapping.size();
  // check, if it's a leave
  if (nodeId < numLeaves) {
    elementsToClusterMapping[nodeId] = clusterLabel;
    return;
  }

  // its an inner node: get childs and continue recursion
  const unsigned int mappedId = nodeId - numLeaves;
  label_leaves(clusterLabel, clusterTree[mappedId].first, clusterTree, elementsToClusterMapping);
  label_leaves(clusterLabel, clusterTree[mappedId].second, clusterTree, elementsToClusterMapping);
}

unsigned int compute_clusters(
    const std::vector<std::pair<uint32_t, uint32_t> >& clusterTree,
    const std::vector<float>& clusterSimilarities,
    const float cutOff,
    std::vector<uint32_t>& elementsToClusterMapping) {

  const unsigned int numNodes = clusterSimilarities.size(); // this arent numclusters. we should rename this variable!!!
  const unsigned int numLeaves = numNodes + 1;

  // clear result container
  elementsToClusterMapping.clear();
  elementsToClusterMapping.resize(numLeaves, 0);

  // find height for tree cut off
  unsigned int treeCutOffLevel = 0;
  bool didCut = false;
  for (treeCutOffLevel = 0; treeCutOffLevel < numNodes; treeCutOffLevel++) {
    if (clusterSimilarities[treeCutOffLevel] < cutOff) {
      didCut = true;
      break;
    }
  }
  if (!didCut) {
    LOG(WARNING) << " did not cut the tree. all points belong to the same cluster";
    // we do not need to do anything with elementsToClusterMapping, as all elements are initialized with 0
    return 1;
  }

  // now we need to label the leafes
  unsigned int clusterId = 0;
  for (unsigned int i = treeCutOffLevel; i < clusterSimilarities.size(); ++i) {
    unsigned int left = clusterTree[i].first;
    unsigned int right = clusterTree[i].second;

    // was already processed?
    if (left < treeCutOffLevel + numLeaves) {
      label_leaves(clusterId, left, clusterTree, elementsToClusterMapping);
      clusterId++;
    }

    // was already processed?
    if (right < treeCutOffLevel + numLeaves) {
      label_leaves(clusterId, right, clusterTree, elementsToClusterMapping);
      clusterId++;
    }
  }

  return clusterId;
}

} /* agglomerative_clustering */
} /* learning */
} /* awesomeness */
