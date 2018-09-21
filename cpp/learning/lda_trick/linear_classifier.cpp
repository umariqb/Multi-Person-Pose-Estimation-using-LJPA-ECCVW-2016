/*
 * linear_classifier.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: lbossard
 */

#include "linear_classifier.hpp"

#include <glog/logging.h>

namespace awesomeness {
namespace learning {
namespace lda_trick {

LinearClassifier::LinearClassifier() {

}

LinearClassifier::~LinearClassifier() {
}

double LinearClassifier::predict(const cv::Mat_<float>& x) const {

  cv::Mat_<double> x_double;
  x.convertTo(x_double, x_double.type());
  return predict(x_double);

}

double LinearClassifier::predict(const cv::Mat_<double>& x) const {

  CHECK((x.rows == 1 && x.cols == _w.cols) || (x.cols == 1 && x.rows == _w.cols));
  return _w.dot(x);

}

} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */
