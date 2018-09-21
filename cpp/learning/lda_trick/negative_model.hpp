/*
 * negative_model.hpp
 *
 *  Created on: Aug 27, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__LDA_TRICK__NEGATIVE_MODEL_HPP_
#define AWESOMENESS__LEARNING__LDA_TRICK__NEGATIVE_MODEL_HPP_

#include <opencv2/core/core.hpp>

#include <boost/serialization/access.hpp>
#include <boost/noncopyable.hpp>

#include "cpp/utils/serialization/opencv_serialization.hpp"

#include "linear_classifier.hpp"

namespace awesomeness {
namespace learning {
namespace lda_trick {


/**
 * Simple negative model for a specific window size.
 */
class NegativeModel : boost::noncopyable {
public:
  NegativeModel();
  virtual ~NegativeModel();

  /**
   * returns a classifier for the provided positives
   * @param positives
   * @param w
   */
  void get_classifier(const cv::Mat_<float>& positives, cv::Mat_<double>* w) const;

  void get_classifier(const cv::Mat_<float>& positives, LinearClassifier* w) const;

  cv::Mat_<double>& mutable_sigma_inv() { return _sigma_inv; }
  cv::Mat_<double>& mutable_mu_neg() { return _mu_neg; }
  const cv::Mat_<double>& sigma_inv() const { return _sigma_inv; }
  const cv::Mat_<double>& mu_neg() const { return _mu_neg; }

private:
  cv::Mat_<double> _sigma_inv;
  cv::Mat_<double> _mu_neg;


  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & _sigma_inv;
    ar & _mu_neg;
  }
};

} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */

#endif /* AWESOMENESS__LEARNING__LDA_TRICK__NEGATIVE_MODEL_HPP_ */
