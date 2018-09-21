/*
 * test_online_covariance.cpp
 *
 *  Created on: 03.09.2013
 *      Author: luk
 */
#include "cpp/third_party/gtest/gtest.h"


#include "cpp/learning/lda_trick/online_covariance.hpp"


void foo(cv::Mat_<float> row, cv::Mat_<float>*rowptr){
  row.copyTo(*rowptr);
}

TEST(OnlineCovarTest, covar) {
  cv::Mat_<double> cv_mat(200,6);
  cv::randu(cv_mat, -10, 10);

  awesomeness::learning::lda_trick::OnlineCovariance online_stat;

  for (int r = 0; r < cv_mat.rows; ++r){
    online_stat.push_sample(cv_mat.row(r));
  }

  cv::Mat_<double> covar;
  cv::Mat_<double> mean;
  const int32_t covar_flags = cv::COVAR_ROWS | cv::COVAR_NORMAL | cv::COVAR_SCALE;
  cv::calcCovarMatrix(cv_mat, covar, mean, covar_flags);

  cv::Mat_<double> online_covar;
  // opencv computes the second moment.
  online_stat.get_covariance(&online_covar, /*second_moment*/ true);
  cv::Mat_<double> online_mean;
  online_stat.get_mean(&online_mean);

  std::cout << covar << std::endl;
  std::cout << online_covar << std::endl;
  std::cout << cv::countNonZero(cv::abs(covar - online_covar) > 0.000001) << std::endl;

  ASSERT_EQ(0, cv::countNonZero(cv::abs(mean - online_mean) > 0.000001));
  ASSERT_EQ(0, cv::countNonZero(cv::abs(covar - online_covar) > 0.000001));

}

