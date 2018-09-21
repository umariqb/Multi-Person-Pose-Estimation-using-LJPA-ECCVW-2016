/*
 * clustering.hpp
 *
 *  Created on: May 26, 2009
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__CLUSTERING_HPP_
#define AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__CLUSTERING_HPP_

#include <vector>
#include <stdint.h>

namespace awesomeness {
namespace learning {
namespace agglomerative_clustering {

unsigned int compute_clusters(
    const std::vector<std::pair<uint32_t, uint32_t> >& clusterTree,
    const std::vector<float>& clusterSimilarities, const float cutOff,
    std::vector<uint32_t>& elementsToClusterMapping);

} /* agglomerative_clustering */
} /* learning */
} /* awesomeness */

#endif /* AWESOMENESS__LEARNING_AGGOMERATIVE_CLUSTERING__CLUSTERING_HPP_ */
