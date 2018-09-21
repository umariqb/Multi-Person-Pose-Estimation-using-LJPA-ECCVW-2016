/*
 * dlib_helpers.hpp
 *
 *  Created on: Feb 17, 2014
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__DLIB_HELPERS_HPP_
#define AWESOMENESS__LEARNING__SVM__DLIB_HELPERS_HPP_


namespace awesomeness {
namespace learning {
namespace svm {
namespace dlib_helpers {

template <typename T, typename S>
void copy_to_sample(const cv::Mat_<T>& row, std::vector<std::pair<uint32_t, S> >* sample_){
  std::vector<std::pair<uint32_t, S> >& sample = *sample_;
  sample.clear();
  // copy features
  for (int col = 0; col < row.cols; ++col) {
    const T& value = row(col);
    if (value != static_cast<S>(0)) {
      DCHECK(!(value != value)) << "is not a number at column" << col;
      sample.push_back(std::make_pair(col, value));
    }
  }
}

template <typename T, typename S>
void copy_to_sample(const cv::Mat_<T>& row, dlib::matrix<S, 1, 0>* sample_){
  dlib::matrix<S, 1, 0>& sample = *sample_;
  sample.set_size(1, row.cols);
  // copy features
  for (int col = 0; col < row.cols; ++col) {
    DCHECK(!(row(col) != row(col)))<< "is not a number at column" << col;
    sample(col) =  row(col);
  }
}

template <typename T, typename S>
void copy_to_sample(const cv::Mat_<T>& row, dlib::matrix<S, 0, 1>* sample_){
  dlib::matrix<S, 0, 1>& sample = *sample_;
  sample.set_size(row.cols, 1);
  // copy features
  for (int col = 0; col < row.cols; ++col) {
    DCHECK(!(row(col) != row(col)))<< "is not a number at column" << col;
    sample(col) =  row(col);
  }
}

} /* namespace dlib_helpers */
} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__DLIB_HELPERS_HPP_ */
