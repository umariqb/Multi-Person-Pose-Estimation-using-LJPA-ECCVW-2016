/*
 * vl_svm_problem.hpp
 *
 *  Created on: Sep 26, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__VL_SVM_PROBLEM_HPP_
#define AWESOMENESS__LEARNING__SVM__VL_SVM_PROBLEM_HPP_

#include "svm_problem.hpp"


namespace awesomeness {
namespace learning {
namespace svm {

struct VlSvmParameters : public SvmParameters {
  enum SolverType {
     Sgd = 0,
     Sdca = 1
   };

  double eps;
  double C;
  SolverType solver_type;
  int max_iter;



  VlSvmParameters(){
    C = 1;
    eps = 0.001;
    solver_type = Sgd;
    max_iter = 0;
  }

  virtual ~VlSvmParameters(){
  }
};

class VlSvmProblem: public SvmProblem {
public:
  VlSvmProblem(
      uint32_t num_classes,
      std::size_t feature_dimensions,
      const VlSvmParameters& svm_parameters);
  virtual ~VlSvmProblem();

  virtual void clear_samples();

  virtual void push_sample(const cv::Mat_<float>& row, const int32_t y);

  virtual Svm* train(
      std::vector<int> weight_label = std::vector<int>(),
      std::vector<double> weights = std::vector<double>());

  virtual inline std::size_t num_samples() const;

private:
  VlSvmParameters _params;
  std::vector<double> _labels;
  std::vector<double> _samples;

  VlSvmProblem() : SvmProblem(0,0){};

  virtual void allocate(
        std::size_t sample_count,
        std::size_t total_nonzero_features_hint=0);

};

//==============================================================================
inline std::size_t  VlSvmProblem::num_samples() const {
  return  _samples.size() / _feature_dimensions;
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__VL_SVM_PROBLEM_HPP_ */
