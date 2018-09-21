/*
 * linkage.hpp
 *
 *  Created on: May 26, 2009
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__LINKAGE_HPP_
#define AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__LINKAGE_HPP_

#include <vector>
#include <map>
#include <limits>

#include <glog/logging.h>

namespace awesomeness {
namespace learning {
namespace agglomerative_clustering {

// convenience functions for linkage choosing
namespace linkage_type{
enum LinkageType {
  SingleLinkage,
  AverageLinkage,
  CompleteLinkage,
};
typedef LinkageType T;
T fromString(const std::string& type);
std::string toString(const T type);
}


/**
 * Computes the linkage for a given similarity matrix (note: similarity = 1/distance)
 * See Matlab reference 'linkage' for further data structure explenation
 * @param similarityMatrix
 * @param clusterTree
 * @param clusterDistances
 * @return
 */
template<class similarity_matrix_T, class linkage_function_T>
bool compute_linkage(
    const similarity_matrix_T& similarityMatrix,
    std::vector<std::pair<unsigned int, unsigned int> >& clusterTree,
    std::vector<float>& clusterSimilarities);

/**
 * @overload
 * @param similarityMatrix
 * @param linkageMethod
 * @param clusterTree
 * @param clusterSimilarities
 * @return
 */
template<class similarity_matrix_T>
bool compute_linkage(
    const similarity_matrix_T& similarityMatrix,
    const linkage_type::T linkageMethod,
    std::vector<std::pair<unsigned int, unsigned int> >& clusterTree,
    std::vector<float>& clusterSimilarities);

/**
 * Averate Linkage:
 * Average similarity between all pairs from cluster1 and cluster2
 */
template<class similarity_matrix_T>
struct AverageLinkage {
  float operator()(
      const std::vector<unsigned int>& cluster1,
      const std::vector<unsigned int>& cluster2,
      const similarity_matrix_T& similarityMatrix) const;
};

/**
 * Single Linkage
 * Find the nearest neighbors: Return the <b>highest</b> similarity
 */
template<class similarity_matrix_T>
struct SingleLinkage {
  float operator( )(
      const std::vector<unsigned int>& cluster1,
      const std::vector<unsigned int>& cluster2,
      const similarity_matrix_T& similarityMatrix) const;
};

/**
 * Complete Linkage:
 * Find the furthest neighbors: Return the <b>lowest</b> similarity
 */
template<class similarity_matrix_T>
struct CompleteLinkage {
  float operator( )(
      const std::vector<unsigned int>& cluster1,
      const std::vector<unsigned int>& cluster2,
      const similarity_matrix_T& similarityMatrix) const;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation
template<class similarity_matrix_T, class linkage_function_T>
bool compute_linkage(
    const similarity_matrix_T& simmilarityMatrix,
    std::vector<std::pair<unsigned int, unsigned int> >& clusterTree,
    std::vector<float>& clusterSimilarities) {

  const unsigned int numPoints = simmilarityMatrix.size1();
  if (simmilarityMatrix.size2() != numPoints) {
    LOG(ERROR) << "similarity matrix is not square";
    return false;
  }
  if (numPoints < 2) {
    LOG(WARNING) << "WARNING: trivial result: only one point provided";
    clusterTree.clear();
    clusterSimilarities.clear();
    return true;
  }

  // resize result containers
  const unsigned int mergingSteps = numPoints - 1;
  clusterTree.resize(mergingSteps);
  clusterSimilarities.resize(mergingSteps);

  // first, all points are one cluster
  typedef std::map<unsigned int, std::vector<unsigned int> > ClusterToPoints;
  ClusterToPoints clustersToPoints;
  for (unsigned int i = 0; i < numPoints; ++i) {
    clustersToPoints[i].push_back(i);
  }

  // start merging
  float maxSimilarity = -1;
  unsigned int maxSimCluster1 = 0, maxSimCluster2 = 0;
  float similarity = 0;
  unsigned int clusterId1 = 0, clusterId2 = 0;
  const linkage_function_T linkageFunction = linkage_function_T();
  for (unsigned int mergeStep = 0; mergeStep < mergingSteps; ++mergeStep) {

    // find the two "nearest" clusters
    maxSimilarity = -1; // similarity needs to be >=0
    maxSimCluster1 = 0;
    maxSimCluster2 = 0;
    for (ClusterToPoints::iterator i = clustersToPoints.begin(); i != clustersToPoints.end(); ++i) {
      for (ClusterToPoints::iterator j = clustersToPoints.begin(); j != clustersToPoints.end(); ++j) {
        clusterId1 = i->first;
        clusterId2 = j->first;
        if (clusterId1 >= clusterId2) {
          continue;
        }
        similarity = linkageFunction(i->second, j->second, simmilarityMatrix);
        if (similarity > maxSimilarity) {
          maxSimilarity = similarity;
          maxSimCluster1 = clusterId1;
          maxSimCluster2 = clusterId2;
        }
      }
    }

    // update clusterTree and clusterDistances
    clusterTree[mergeStep].first = maxSimCluster1;
    clusterTree[mergeStep].second = maxSimCluster2;
    clusterSimilarities[mergeStep] = maxSimilarity;

    // merge the two clusters
    {
      ClusterToPoints::mapped_type& points = clustersToPoints[mergeStep + numPoints];
      points.swap(clustersToPoints[maxSimCluster1]);
      points.insert(
          points.end(),
          clustersToPoints[maxSimCluster2].begin(),
          clustersToPoints[maxSimCluster2].end());
      clustersToPoints.erase(maxSimCluster1);
      clustersToPoints.erase(maxSimCluster2);
    }
  }
  return true;
}
//------------------------------------------------------------------------------
template<class similarity_matrix_T>
bool compute_linkage(
    const similarity_matrix_T& distanceMatrix,
    const linkage_type::T linkageMethod,
    std::vector<std::pair<unsigned int, unsigned int> >& clusterTree,
    std::vector<float>& clusterDistances) {

  switch (linkageMethod) {
    case linkage_type::SingleLinkage:
      return compute_linkage<similarity_matrix_T,
          SingleLinkage<similarity_matrix_T> >(distanceMatrix, clusterTree,
          clusterDistances);
      break;
    case linkage_type::AverageLinkage:
      return compute_linkage<similarity_matrix_T,
          AverageLinkage<similarity_matrix_T> >(distanceMatrix, clusterTree,
          clusterDistances);
      break;
    case linkage_type::CompleteLinkage:
      return compute_linkage<similarity_matrix_T,
          CompleteLinkage<similarity_matrix_T> >(distanceMatrix, clusterTree,
          clusterDistances);
      break;
  }
  return false;
}
//------------------------------------------------------------------------------

template<class similarity_matrix_T>
float AverageLinkage<similarity_matrix_T>::operator()(
    const std::vector<unsigned int>& cluster1,
    const std::vector<unsigned int>& cluster2,
    const similarity_matrix_T& similarityMatrix) const {

  float similarity = 0.0f;
  const unsigned int numPointsC1 = cluster1.size();
  const unsigned int numPointsC2 = cluster2.size();
  for (unsigned int i = 0; i < numPointsC1; ++i) {
    for (unsigned int j = 0; j < numPointsC2; ++j) {
      similarity += similarityMatrix(cluster1[i], cluster2[j]);
    }
  }
  similarity /= (numPointsC1 + numPointsC2);
  return similarity;
}
//------------------------------------------------------------------------------
/**
 * Single Linkage
 * Find the nearest neighbors: Return the <b>highest</b> similarity
 */
template<class similarity_matrix_T>
float SingleLinkage<similarity_matrix_T>::operator( )(
    const std::vector<unsigned int>& cluster1,
    const std::vector<unsigned int>& cluster2,
    const similarity_matrix_T& similarityMatrix) const {

  float similarity = -1.f; // lowest possible similarity would be 0.
  const unsigned int numPointsC1 = cluster1.size();
  const unsigned int numPointsC2 = cluster2.size();
  for (unsigned int i = 0; i < numPointsC1; ++i) {
    for (unsigned int j = 0; j < numPointsC2; ++j) {
      similarity = std::max(
          similarity,
          (float) similarityMatrix(cluster1[i], cluster2[j]));
    }
  }
  return similarity;
}
//------------------------------------------------------------------------------
/**
 * Complete Linkage:
 * Find the furthest neighbors: Return the <b>lowest</b> similarity
 */
template<class similarity_matrix_T>
float CompleteLinkage<similarity_matrix_T>::operator( )(
    const std::vector<unsigned int>& cluster1,
    const std::vector<unsigned int>& cluster2,
    const similarity_matrix_T& similarityMatrix) const {

  float similarity = std::numeric_limits<float>::max();
  const unsigned int numPointsC1 = cluster1.size();
  const unsigned int numPointsC2 = cluster2.size();
  for (unsigned int i = 0; i < numPointsC1; ++i) {
    for (unsigned int j = 0; j < numPointsC2; ++j) {
      similarity = std::min(
          similarity,
          static_cast<float>(similarityMatrix(cluster1[i], cluster2[j])));
    }
  }
  return similarity;
}

} /* agglomerative_clustering */
} /* learning */
} /* awesomeness */

#endif /* AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__LINKAGE_HPP_ */
