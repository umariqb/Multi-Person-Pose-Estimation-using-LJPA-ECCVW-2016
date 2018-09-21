/*
 * negative_model.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: lbossard
 */

#include "negative_model.hpp"

#include <glog/logging.h>

#include <opencv2/core/core_c.h> // CV_REDUCE_AVG

namespace awesomeness {
namespace learning {
namespace lda_trick {

NegativeModel::NegativeModel() {

}

NegativeModel::~NegativeModel() {
}


void NegativeModel::get_classifier(const cv::Mat_<float>& positives, LinearClassifier* w) const {
  get_classifier(positives, &(w->mutable_w()));
}

void NegativeModel::get_classifier(const cv::Mat_<float>& positives, cv::Mat_<double>* w_) const {

  CHECK_EQ(positives.cols, _mu_neg.cols) << "input vector has the wrong dimensions";
  CHECK_GE(positives.rows, 1) << "need at least one positive example";

  cv::Mat_<double> mu_pos;
  static const int reduce_dimension_row_wise = 0;
  cv::reduce(positives, mu_pos, reduce_dimension_row_wise, CV_REDUCE_AVG, mu_pos.type());

  cv::Mat_<double>& w = *w_;
  w = _sigma_inv * (mu_pos - _mu_neg);

}


} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */
