/*
 * vl_svm_problem.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: lbossard
 */

#include "vl_svm_problem.hpp"

#include <boost/scoped_ptr.hpp>
#include <glog/logging.h>

extern "C" {
#include "cpp/third_party/vlfeat/vl/svm.h"
}

#include "vl_svm.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

VlSvmProblem::VlSvmProblem(
    uint32_t num_classes,
    std::size_t feature_dimensions,
    const VlSvmParameters& svm_parameters)
: SvmProblem(num_classes, feature_dimensions),
  _params(svm_parameters)
    {

}


/*virtual*/ VlSvmProblem::~VlSvmProblem(){

}


/*virtual*/ void VlSvmProblem::clear_samples(){
  _labels.clear();
  _samples.clear();
}


/*virtual*/ void VlSvmProblem::push_sample(const cv::Mat_<float>& row, const int32_t y){

  CHECK(y == 0 || y == 1) << "supporting only binary svm atm";

  const std::size_t start_idx = _samples.size();

  // allocate space
  _samples.resize(start_idx + _feature_dimensions);
  CHECK_EQ(row.cols, _feature_dimensions);
  DCHECK_EQ(0, _samples.size() % _feature_dimensions);

  // copy data
  if (row.isContinuous()){
    const float* row_ptr = row[0];
    for (std::size_t i = 0; i < _feature_dimensions; ++i){
      _samples[start_idx + i] = *row_ptr;
      ++row_ptr;
    }
  }
  else {
    for (std::size_t i = 0; i < _feature_dimensions; ++i){
      _samples[start_idx + i] = row(i);
    }
  }

  // safe label
  _labels.push_back(y);

}


/*virtual*/ Svm* VlSvmProblem::train(
    std::vector<int> weight_label,
    std::vector<double> weights){

  VlSvmSolverType solver_type;
  switch (_params.solver_type){
    case VlSvmParameters::Sgd:
      solver_type = VlSvmSolverSgd;
      break;
    case VlSvmParameters::Sdca:
      solver_type = VlSvmSolverSdca;
      break;
    // no default!
  }

  // http://www.vlfeat.org/sandbox/api/svm-advanced.html#svm-C
  const double lambda = 1./ (num_samples() * _params.C);

  std::vector<double> vl_feat_labels(_labels.size(), 0);
  for (std::size_t i = 0; i < _labels.size(); ++ i){
    if (_labels[i] == 0){
      vl_feat_labels[i] = 1;
    }
    else {
      vl_feat_labels[i] = -1;
    }
  }
  ::VlSvm* vedaldi_svm = vl_svm_new(
        solver_type,      // sovler type
        _samples.data(),     // ptr to data
        _feature_dimensions, // feature dimensions
        num_samples(),       // number of samples
        vl_feat_labels.data(),      // ptr to data
        lambda
    );
  vl_svm_set_epsilon(vedaldi_svm, _params.eps);
  if (_params.max_iter > 0){
    vl_svm_set_max_num_iterations(vedaldi_svm, _params.max_iter);
  }
  vl_svm_train(vedaldi_svm);

  std::auto_ptr<awesomeness::learning::svm::VlSvm> vl_svm(
      new awesomeness::learning::svm::VlSvm(_feature_dimensions, _label_idx_map));
  vl_svm->set_b(vl_svm_get_bias(vedaldi_svm));

  // copy weight vector
  CHECK_EQ(_feature_dimensions, vl_svm_get_dimension(vedaldi_svm));
  double const * vl_w = vl_svm_get_model(vedaldi_svm);
  cv::Mat_<double>& w = vl_svm->mutable_w();
  w.create(1, _feature_dimensions);
  for (std::size_t i = 0; i < _feature_dimensions; ++i){
    w(i) = vl_w[i];
  }

  vl_svm_delete(vedaldi_svm);

  return vl_svm.release();
}


/*virtual*/ void VlSvmProblem::allocate(
      std::size_t sample_count,
      std::size_t total_nonzero_features_hint){
  _labels.reserve(sample_count);
  _samples.reserve(sample_count * _feature_dimensions);
}


} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
