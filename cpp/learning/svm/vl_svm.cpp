/*
 * vl_svm.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: lbossard
 */

#include "vl_svm.hpp"

#include "cpp/utils/serialization/opencv_serialization.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

VlSvm::VlSvm()
: Svm(0, LabelIdxMap()){
  _b = 0;

}

VlSvm::VlSvm(std::size_t feature_dimensions, const LabelIdxMap& label_idx_map)
:Svm(feature_dimensions, label_idx_map){
  _b = 0;
}

VlSvm::~VlSvm() {
}


/*virtual*/ int32_t VlSvm::predict(const cv::Mat_<float>& features, std::vector<double>* values) const {
  cv::Mat_<double> x_double;
  features.convertTo(x_double, x_double.type());
  const double val = _w.dot(x_double) + _b;
  values->resize(2);
  (*values)[0] = val;
  (*values)[1] = -val;
  if (val >= 0){
    return 0;
  }
  else {
    return 1;
  }
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(awesomeness::learning::svm::VlSvm);
