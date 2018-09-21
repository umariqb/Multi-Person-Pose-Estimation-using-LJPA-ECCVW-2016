/*
 * online_covariance.h
 *
 *  Created on: Sep 6, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__LDA_TRICK__ONLINE_COVARIANCE_H_
#define AWESOMENESS__LEARNING__LDA_TRICK__ONLINE_COVARIANCE_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include <opencv2/core/core.hpp>

#include <armadillo>

#include "cpp/utils/armadillo.hpp"


namespace awesomeness {
namespace learning {
namespace lda_trick {

class OnlineCovariance : boost::noncopyable{
public:

  OnlineCovariance();

  ~OnlineCovariance();

  template <typename T>
  void push_sample(const cv::Mat_<T>& sample);

  void get_covariance(cv::Mat_<double>* covar, bool second_moment=false);

  void get_mean(cv::Mat_<double>* mean);

private:
  arma::running_stat_vec<double> _stats;

  void _wrapper(boost::function<void(cv::Mat_<double>*)> extractor );

};

//==============================================================================
template <typename T>
void OnlineCovariance::push_sample(const cv::Mat_<T>& sample){
  arma::rowvec double_feat;
  utils::armadillo::from_opencv(sample, &double_feat);
  _stats(double_feat);
}

} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__LDA_TRICK__ONLINE_COVARIANCE_H_ */
