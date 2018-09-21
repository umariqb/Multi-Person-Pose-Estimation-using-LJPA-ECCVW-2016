/*
 * simple_svm_problem.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#include "liblinear_svm_problem.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

LibLinearSvmProblem::LibLinearSvmProblem(
    uint32_t num_classes,
    std::size_t feature_dimensions,
    const LibLinearSvmParameters& params)
:
   SvmProblem(num_classes, feature_dimensions),
  _params(params)
{
}

LibLinearSvmProblem::LibLinearSvmProblem():
   SvmProblem(0, 0)
{
}


/*virtual*/LibLinearSvmProblem::~LibLinearSvmProblem() {
}

/*virtual*/void LibLinearSvmProblem::allocate(
    std::size_t sample_count,
    std::size_t total_nonzero_features_hint)
{
  _weights.create(1, num_classes());
  _weights = 0;
  _problem.allocate(sample_count, total_nonzero_features_hint);
}

/*virtual*/void LibLinearSvmProblem::clear_samples() {
  _problem.clear();
  _weights = 0;
}

/*virtual*/void LibLinearSvmProblem::push_sample(
    const cv::Mat_<float>& row,
    const int32_t y) {
  _problem.push_problem(row, y, utils::libsvm::IdentityFunction());
  ++_weights(y);
}

/*virtual*/ Svm* LibLinearSvmProblem::train(std::vector<int> weight_label,
    std::vector<double> weights){

  std::auto_ptr<LibLinearSvm> svm(new LibLinearSvm(_feature_dimensions, _label_idx_map));
  // train
  if(weight_label.size() > 0 && weights.size() > 0 ) {
    svm->mutable_svm().train(_problem, _params.solver_type, _params.C, _params.eps, weights, weight_label);
  }else{
    svm->mutable_svm().train(_problem, _params.solver_type, _params.C, _params.eps);
  }
  return svm.release();
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
