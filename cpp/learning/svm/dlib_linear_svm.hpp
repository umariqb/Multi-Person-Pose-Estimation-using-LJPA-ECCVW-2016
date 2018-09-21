/*
 * dlib_linear_svm.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_HPP_
#define AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_HPP_

#include <dlib/svm/one_vs_all_decision_function.h>
#include <dlib/svm/one_vs_all_trainer.h>

#include "cpp/utils/serialization/dlib_serialization.hpp"

#include "svm.hpp"
#include "dlib_helpers.hpp"

namespace awesomeness {
namespace learning {
namespace svm {


struct SparseDlibLinearSvmTrait {
  typedef double ScalarType;
  typedef std::vector<std::pair<uint32_t, ScalarType> > SampleType;
  typedef dlib::sparse_linear_kernel<SampleType> OneVsAllSvmKernel;

};

struct DenseDlibLinearSvmTrait {
  typedef double ScalarType;
  typedef dlib::matrix<ScalarType, 0, 1> SampleType; // row vector
  typedef dlib::linear_kernel<SampleType> OneVsAllSvmKernel;
};

template <typename StructSvmTrait>
class _DlibLinearSvm : public Svm {
public:
  typedef typename StructSvmTrait::ScalarType ScalarType;
  typedef typename StructSvmTrait::SampleType SampleType;
  typedef typename StructSvmTrait::OneVsAllSvmKernel OneVsAllSvmKernel;

  typedef dlib::any_trainer<SampleType, ScalarType> SingleSvmTrainer;
  typedef dlib::one_vs_all_trainer<SingleSvmTrainer, LabelIndexType > OneVsAllTrainer;
  typedef dlib::one_vs_all_decision_function<
      OneVsAllTrainer,
      dlib::decision_function<OneVsAllSvmKernel> > LinearOneVsAllSvm;


  _DlibLinearSvm(std::size_t feature_dimensions, const LabelIdxMap& label_idx_map);
  virtual ~_DlibLinearSvm();

  virtual int32_t predict(const cv::Mat_<float>& features, std::vector<double>* values) const;

  template <typename T>
  int32_t predict(const cv::Mat_<T>& features, std::vector<double>* values) const;

  int32_t predict(const SampleType& features, std::vector<double>* values) const;

  using Svm::predict;

  inline const LinearOneVsAllSvm& svm() const;
  inline LinearOneVsAllSvm& mutable_svm();

  _DlibLinearSvm();
protected:
  LinearOneVsAllSvm _svm;

private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Svm>(*this);
    ar & _svm;
  }
};


typedef _DlibLinearSvm<DenseDlibLinearSvmTrait> DlibLinearSvm;
typedef _DlibLinearSvm<SparseDlibLinearSvmTrait> SparseDlibLinearSvm;

////////////////////////////////////////////////////////////////////////////////


template <typename T>
_DlibLinearSvm<T>::_DlibLinearSvm()
: Svm(0, LabelIdxMap()){

}

template <typename T>
_DlibLinearSvm<T>::_DlibLinearSvm(
    std::size_t feature_dimensions,
    const LabelIdxMap& label_idx_map)
: Svm(feature_dimensions, label_idx_map)
  {

}

template <typename T>
_DlibLinearSvm<T>::~_DlibLinearSvm(){

}

template <typename T>
/*virtual*/ int32_t _DlibLinearSvm<T>::predict(const cv::Mat_<float>& features, std::vector<double>* values) const {
  SampleType svm_features;
  dlib_helpers::copy_to_sample(features, &svm_features);
  return predict(svm_features, values);

}

template <typename T>
int32_t _DlibLinearSvm<T>::predict(const SampleType& sample, std::vector<double>* values_) const {

  std::vector<double>& values = *values_;
  values.resize(_svm.number_of_classes());
  LabelIndexType best_label = -1;
  ScalarType best_score = -std::numeric_limits<ScalarType>::infinity();

  typename LinearOneVsAllSvm::binary_function_table dfs = _svm.get_binary_decision_functions();

  // run all the classifiers over the sample and find the best one
  for(typename LinearOneVsAllSvm::binary_function_table::const_iterator i = dfs.begin(); i != dfs.end(); ++i){
    int32_t class_id = i->first;
    const ScalarType predicted_score = i->second(sample);
    values[class_id] = predicted_score;

    if (predicted_score > best_score){
      best_score = predicted_score;
      best_label = class_id;
    }
  }
  return best_label;
}

template <typename T>
template <typename TT>
int _DlibLinearSvm<T>::predict(
    const cv::Mat_<TT>& features,
    std::vector<double>* values) const {

  SampleType sample;
  dlib_helpers::copy_to_sample(features, &sample);
  return predict(sample, values);
}

template <typename T>
/*inline*/ const typename _DlibLinearSvm<T>::LinearOneVsAllSvm&
_DlibLinearSvm<T>::svm() const
{
  return _svm;
}

template <typename T>
/*inline*/ typename _DlibLinearSvm<T>::LinearOneVsAllSvm&
_DlibLinearSvm<T>::mutable_svm()
{
  return _svm;
}
} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SDLIB_LINEAR_SVM_HPP_ */
