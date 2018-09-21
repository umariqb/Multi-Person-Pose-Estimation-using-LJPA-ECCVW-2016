/*
 * Linkage.cpp
 *
 *  Created on: May 26, 2009
 *      Author: lbossard
 */

#include "linkage.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace awesomeness {
namespace learning {
namespace agglomerative_clustering {
namespace linkage_type {
T fromString(const std::string& type) {
  if ("single" == type) {
    return SingleLinkage;
  }
  else if ("average" == type) {
    return AverageLinkage;
  }
  else if ("complete" == type) {
    return CompleteLinkage;
  }
  else {
    throw std::runtime_error("unknown linkage type '" + type + "'");
  }
}
std::string toString(const T type) {
  switch (type) {
    case SingleLinkage:
      return "single";
      break;
    case AverageLinkage:
      return "average";
      break;
    case CompleteLinkage:
      return "complete";
      break;
    default:
      throw std::runtime_error(
          "unknown linkage type id '" + boost::lexical_cast<std::string>(type)
              + "'");
  }
}
}

} /* agglomerative_clustering */
} /* learning */
} /* awesomeness */
