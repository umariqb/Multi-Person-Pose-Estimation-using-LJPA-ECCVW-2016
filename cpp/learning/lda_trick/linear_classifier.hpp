/*
 * linear_classifier.hpp
 *
 *  Created on: Aug 27, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__LDA_TRICK__LINEAR_CLASSIFIER_HPP_
#define AWESOMENESS__LEARNING__LDA_TRICK__LINEAR_CLASSIFIER_HPP_

#include <opencv2/core/core.hpp>

#include <boost/serialization/access.hpp>
#include <boost/noncopyable.hpp>

#include "cpp/utils/serialization/opencv_serialization.hpp"


namespace awesomeness {
namespace learning {
namespace lda_trick {

class LinearClassifier {
public:
  LinearClassifier();
  virtual ~LinearClassifier();

  double predict(const cv::Mat_<float>& x) const;

  double predict(const cv::Mat_<double>& x) const;

  cv::Mat_<double>& mutable_w() { return _w;}
  const cv::Mat_<double>& w() const {return _w;}

private:
  cv::Mat_<double> _w;


  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /*version*/) {
    ar & _w;
  }

};

} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__LDA_TRICK__LINEAR_CLASSIFIER_HPP_ */
