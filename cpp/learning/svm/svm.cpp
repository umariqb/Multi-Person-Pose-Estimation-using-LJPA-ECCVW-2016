/*
 * svm.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#include "svm.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

Svm::~Svm(){
}

Svm::Svm(
    std::size_t feature_dimensions,
    const LabelIdxMap& label_idx_map
    ){
  _label_idx_map = label_idx_map;
  _num_classes = label_idx_map.size();
  _feature_dimensions = feature_dimensions;
}

Svm::Svm() {
  _num_classes = 0;
  _feature_dimensions = 0;
}

int32_t Svm::predict(const cv::Mat_<float>& features) const {
  std::vector<double> values;
  return predict(features, &values);
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
