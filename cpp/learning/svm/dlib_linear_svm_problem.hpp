/*
 * dlib_linear_svm_problem.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_PROBLEM__HPP_
#define AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_PROBLEM__HPP_

#include <dlib/svm_threaded.h>
#include <glog/logging.h>

#include "svm_problem.hpp"
#include "dlib_linear_svm.hpp"

#ifdef DLIB_USE_BLAS
extern "C" void openblas_set_num_threads(int num_threads);
#endif

namespace awesomeness {
namespace learning {
namespace svm {


struct DlibLinearSvmParameters : public SvmParameters {
  enum SolverType {
    DualCoordinateDescent,
    Oca,
    CsvmSmo,
    NuSvm,
//    Pegasos,
  };

  SolverType solver_type;
  uint32_t num_threads;
  double C;
  double eps;
  double nu;
  bool is_sparse;
  bool is_verbose;
  int max_iterations;
  bool use_bias;


  virtual ~DlibLinearSvmParameters(){};
  DlibLinearSvmParameters(){
    solver_type = CsvmSmo;
    num_threads = 1;
    C = 10;
    eps = 0.001;
    nu = 0;
    is_sparse = false;
    is_verbose = true;
    max_iterations = 10000;
    use_bias = true;
  }
};


template <typename DlibLinearSvmTrait>
class _DlibLinearSvmProblem : public SvmProblem {
  typedef _DlibLinearSvm<DlibLinearSvmTrait> SvmType;
  typedef typename SvmType::SampleType SampleType;
  typedef typename SvmType::ScalarType ScalarType;
  typedef std::vector<typename Svm::LabelIndexType> LabelVector;
  typedef std::vector<SampleType> SampleVector;

public:
  _DlibLinearSvmProblem(
      uint32_t num_classes,
      std::size_t feature_dimensions,
      const DlibLinearSvmParameters& svm_parameters);

  virtual ~_DlibLinearSvmProblem();

  virtual void clear_samples();

  virtual void push_sample(const cv::Mat_<float>& row, const int32_t y);
  virtual std::size_t num_samples() const {
    return _labels.size();
  }

  virtual Svm* train(std::vector<int> weight_label = std::vector<int>(),
      std::vector<double> weights = std::vector<double>());


protected:
  DlibLinearSvmParameters _params;
  LabelVector _labels;
  SampleVector _samples;

  virtual void allocate(
      std::size_t sample_count,
      std::size_t total_nonzero_features_hint=0);
private:
  _DlibLinearSvmProblem();
};

typedef _DlibLinearSvmProblem<SparseDlibLinearSvmTrait> SparseDlibLinearSvmProblem;
typedef _DlibLinearSvmProblem<DenseDlibLinearSvmTrait> DlibLinearSvmProblem;

////////////////////////////////////////////////////////////////////////////////
template <typename T>
_DlibLinearSvmProblem<T>::_DlibLinearSvmProblem() {
}

template <typename T>
_DlibLinearSvmProblem<T>::_DlibLinearSvmProblem(
    uint32_t num_classes,
    std::size_t feature_dimensions,
    const DlibLinearSvmParameters& svm_parameters)
  :
    SvmProblem(num_classes, feature_dimensions),
    _params(svm_parameters)
{

}

template <typename T>
_DlibLinearSvmProblem<T>::~_DlibLinearSvmProblem() {
}

template <typename T>
void _DlibLinearSvmProblem<T>::clear_samples(){
  _labels.clear();
  _samples.clear();
}

template <typename T>
void _DlibLinearSvmProblem<T>::push_sample(
    const cv::Mat_<float>& row, const int32_t y) {
  CHECK(y < _num_classes);
  _labels.push_back(y);
  // add another sample and get its reference
  _samples.resize(_samples.size() + 1);
  SampleType& sample = *_samples.rbegin();

  dlib_helpers::copy_to_sample(row, &sample);
}

template <typename T>
Svm* _DlibLinearSvmProblem<T>::train(
    std::vector<int> weight_label, std::vector<double> weights) {

#ifdef DLIB_USE_BLAS
  openblas_set_num_threads(1);
#endif

  std::map<int, double> weight_map;
  if (weight_label.size() > 0){
    CHECK_EQ(weight_label.size(), num_classes());
    CHECK_EQ(weights.size(), num_classes());
    for (int idx = 0; idx < weight_label.size(); ++idx){
      weight_map[weight_label[idx]] = weights[idx];
    }
  }


  std::auto_ptr<SvmType > linear_svm(new SvmType(_feature_dimensions, _label_idx_map));
  typename SvmType::LinearOneVsAllSvm& svm = linear_svm->mutable_svm();
  typename SvmType::OneVsAllTrainer trainer;


  switch (_params.solver_type){
    case DlibLinearSvmParameters::DualCoordinateDescent:
    {
      typedef dlib::svm_c_linear_dcd_trainer<typename SvmType::OneVsAllSvmKernel> SingleTrainer;
      trainer.set_trainer(SingleTrainer());

      for (int label = 0; label < num_classes(); ++label){
        SingleTrainer dcd_trainer;

        dcd_trainer.set_epsilon(_params.eps);

        dcd_trainer.set_max_iterations(_params.max_iterations);
        if (_params.is_verbose){
          dcd_trainer.be_verbose();
        }

        dcd_trainer.set_c(_params.C);
        if (weight_label.size() > 0){
          dcd_trainer.set_c_class1(_params.C * weight_map.at(label));
        }

        trainer.set_trainer(dcd_trainer, label);
      }
      trainer.set_num_threads(_params.num_threads);

      svm = trainer.train(_samples, _labels);

    }
    break;
    case DlibLinearSvmParameters::Oca:
    {

      typedef dlib::svm_c_linear_trainer<typename SvmType::OneVsAllSvmKernel> SingleTrainer;
      trainer.set_trainer(SingleTrainer());

      for (int label = 0; label < num_classes(); ++label){
        SingleTrainer single_trainer;

        single_trainer.set_epsilon(_params.eps);
        single_trainer.set_max_iterations(_params.max_iterations);
        if (_params.is_verbose){
          single_trainer.be_verbose();
        }

        single_trainer.set_c(_params.C);
        if (weight_label.size() > 0){
          single_trainer.set_c_class1(_params.C * weight_map.at(label));
        }
        trainer.set_trainer(single_trainer, label);
      }
      trainer.set_num_threads(_params.num_threads);
      svm = trainer.train(_samples, _labels);
    }
    break;
    case DlibLinearSvmParameters::CsvmSmo:
    {
      typedef dlib::svm_c_trainer<typename SvmType::OneVsAllSvmKernel> SingleTrainer;
      trainer.set_trainer(SingleTrainer());

      for (int label = 0; label < num_classes(); ++label){
        SingleTrainer single_trainer;

        single_trainer.set_epsilon(_params.eps);

        single_trainer.set_c(_params.C);
        if (weight_label.size() > 0){
          single_trainer.set_c_class1(_params.C * weight_map.at(label));
        }

        trainer.set_trainer(single_trainer, label);
      }
      trainer.set_num_threads(_params.num_threads);
      svm = trainer.train(_samples, _labels);
    }
    break;
    case DlibLinearSvmParameters::NuSvm:
    {
      typedef dlib::svm_nu_trainer<typename SvmType::OneVsAllSvmKernel> SingleTrainer;
      trainer.set_trainer(SingleTrainer());

      for (int label = 0; label < num_classes(); ++label){
        SingleTrainer single_trainer;
        single_trainer.set_epsilon(_params.eps);
        trainer.set_trainer(single_trainer, label);
      }
      trainer.set_num_threads(_params.num_threads);
      svm = trainer.train(_samples, _labels);
    }
    break;
//    case DlibLinearSvmParameters::Pegasos:
//    {
//      typedef dlib::svm_pegasos<typename SvmType::OneVsAllSvmKernel> SingleTrainer;
//      trainer.set_trainer(SingleTrainer());
//
//      const double lambda = 1 / (_params.C * _samples.size());
//
//      for (int label = 0; label < num_classes(); ++label){
//        SingleTrainer single_trainer;
//        single_trainer.set_lambda(lambda);
//        if (weight_label.size() > 0){
//          single_trainer.set_lambda_class1(1 / (_params.C * _samples.size() * weight_map.at(label)) );
//        }
//        single_trainer.set_tolerance(_params.eps);
//        trainer.set_trainer(single_trainer, label);
//      }
//      trainer.set_num_threads(_params.num_threads);
//      svm = trainer.train(_samples, _labels);
//    }
//    break;
  }




  return linear_svm.release();
}

template <typename T>
void _DlibLinearSvmProblem<T>::allocate(
    std::size_t sample_count, std::size_t total_nonzero_features_hint) {
  _samples.reserve(sample_count);
}



} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_PROBLEM__HPP_ */
