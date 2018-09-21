/*
 * simple_svm.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#include "liblinear_svm.hpp"

#include <boost/foreach.hpp>

#include "cpp/vision/features/spmbow_extractor.hpp"
#include "cpp/utils/stl_utils.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

LibLinearSvm::LibLinearSvm()
: Svm(0, LabelIdxMap()){

}
LibLinearSvm::LibLinearSvm(
    std::size_t feature_dimensions,
    const LabelIdxMap& label_idx_map)
: Svm(feature_dimensions, label_idx_map)
  {

}
LibLinearSvm::~LibLinearSvm(){

}



/*virtual*/ int32_t LibLinearSvm::predict(const cv::Mat_<float>& features, std::vector<double>* values) const {
  std::vector<utils::liblinear::FeatureNode> liblinear_features;
  liblinear_features.clear();
  utils::liblinear::convert_linear_features(
      features,
      liblinear_features,
      utils::libsvm::IdentityFunction());
  return _svm.predict(liblinear_features, *values);
}

void LibLinearSvm::get_weights(std::vector<double>& weights) const {
  _svm.get_weights(weights);
}




} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(awesomeness::learning::svm::LibLinearSvm);
