/*
 * svm_problem.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SVM_PROBLEM_HPP_
#define AWESOMENESS__LEARNING__SVM__SVM_PROBLEM_HPP_


#include <boost/noncopyable.hpp>

#include "svm.hpp"

namespace awesomeness {
namespace learning {
namespace svm {


/**
 * struct representing the svm specific parameters
 */
struct SvmParameters {
  SvmParameters();
  virtual ~SvmParameters();
};


/**
 * SvmProblem used to hold the training data
 */
class SvmProblem : boost::noncopyable {

public:
  typedef Svm::LabelIdxMap LabelIdxMap;

  static SvmProblem* create(
      const SvmParameters& parameters,
      uint32_t num_classes,
      std::size_t feature_dimensions,
      std::size_t sample_count_hint=0,
      std::size_t total_nonzero_features_hint=0);

  static SvmProblem* create(
      const SvmParameters& parameters,
      const std::vector<std::string>& labels,
      std::size_t feature_dimensions,
      std::size_t sample_count_hint=0,
      std::size_t total_nonzero_features_hint=0);

  virtual ~SvmProblem();

  void push_sample(const cv::Mat_<float>& row, const std::string& label);

  virtual void clear_samples() = 0;

  virtual void push_sample(const cv::Mat_<float>& row, const int32_t y) = 0;

  virtual std::size_t num_samples() const = 0;

  virtual Svm* train(std::vector<int> weight_label = std::vector<int>(),
                     std::vector<double> weights = std::vector<double>() ) = 0;

  inline std::size_t feature_dimensions() const { return _feature_dimensions;};
  inline uint32_t num_classes() const {return _num_classes;};

protected:
  LabelIdxMap _label_idx_map;
  std::size_t _feature_dimensions;
  uint32_t _num_classes;

  SvmProblem(uint32_t num_classes, std::size_t feature_dimensions);

  virtual void allocate(
      std::size_t sample_count,
      std::size_t total_nonzero_features_hint=0) = 0;


  void create_label_idx_map(const std::vector<std::string>& labels);

private:
  SvmProblem();

};



} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SVM_PROBLEM_HPP_ */
