/*
 * simple_svm_problem.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_PROBLEM_HPP_
#define AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_PROBLEM_HPP_


#include "liblinear_svm.hpp"
#include "svm_problem.hpp"

namespace awesomeness {
namespace learning {
namespace svm {


struct LibLinearSvmParameters : public SvmParameters {
  utils::liblinear::solver_type::T solver_type;
  double C;
  double eps;

  LibLinearSvmParameters(){
    C = 1;
    solver_type = (utils::liblinear::solver_type::T)1;
    eps = 0.1;
  }

  virtual ~LibLinearSvmParameters(){

  }
};

class LibLinearSvmProblem : public SvmProblem {
public:
  LibLinearSvmProblem(
      uint32_t num_classes,
      std::size_t feature_dimensions,
      const LibLinearSvmParameters& svm_parameters);

  virtual ~LibLinearSvmProblem();

  virtual void allocate(std::size_t sample_count, std::size_t total_nonzero_features_hint=0);


  virtual void clear_samples();

  virtual void push_sample(const cv::Mat_<float>& row, const int32_t y);

  virtual std::size_t num_samples() const {
    return _problem.liblinear_problem().l;
  }

  virtual Svm* train(std::vector<int> weight_label = std::vector<int>(),
      std::vector<double> weights = std::vector<double>());

protected:
  LibLinearSvmProblem();

private:
  utils::liblinear::ProblemHolder _problem;
  cv::Mat_<double> _weights;
  LibLinearSvmParameters _params;
};

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_PROBLEM_HPP_ */
