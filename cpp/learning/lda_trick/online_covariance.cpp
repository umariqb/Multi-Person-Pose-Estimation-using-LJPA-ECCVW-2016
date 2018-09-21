/*
 * online_covariance.cpp
 *
 *  Created on: Sep 6, 2013
 *      Author: lbossard
 */

#include "online_covariance.hpp"

#include "cpp/utils/armadillo.hpp"

namespace awesomeness {
namespace learning {
namespace lda_trick {

OnlineCovariance::~OnlineCovariance() {
}

OnlineCovariance::OnlineCovariance()
: _stats(/*calc_cov*/ true)
{

}

void OnlineCovariance::get_covariance(cv::Mat_<double>* covar, bool compute_second_moment){
  utils::armadillo::to_opencv(_stats.cov((int)compute_second_moment), covar);
}

void OnlineCovariance::get_mean(cv::Mat_<double>* mean){
  utils::armadillo::to_opencv(_stats.mean(), mean);
}

} /* namespace lda_trick */
} /* namespace learning */
} /* namespace awesomeness */
