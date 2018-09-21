/*
 * struct_svm.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */
#ifndef AWESOMENESS__LEARNING__SVM__STRUCT_SVM_H_
#define AWESOMENESS__LEARNING__SVM__STRUCT_SVM_H_

#include <vector>
#include <dlib/svm/function.h>
#include <dlib/svm/structural_svm_problem_threaded.h>

#include <glog/logging.h>

#include <boost/serialization/map.hpp>
#include "cpp/utils/serialization/dlib_serialization.hpp"

#include "svm_problem.hpp"
#include "dlib_helpers.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

struct SparseStructSvmTrait {
  typedef double ScalarType;
  typedef std::vector<std::pair<uint32_t, ScalarType> > SampleType;
  typedef dlib::sparse_linear_kernel<SampleType> StructSvmKernel;

};

struct SparseStructSvmTraitFloat {
  typedef float ScalarType;
  typedef std::vector<std::pair<uint32_t, ScalarType> > SampleType;
  typedef dlib::sparse_linear_kernel<SampleType> StructSvmKernel;

};

struct DenseStructSvmTrait {
  typedef double ScalarType;
  typedef dlib::matrix<ScalarType, 1, 0> SampleType; // row vector
  typedef dlib::linear_kernel<SampleType> StructSvmKernel;
};

struct DenseStructSvmTraitFloat {
  typedef float ScalarType;
  typedef dlib::matrix<ScalarType, 1, 0> SampleType; // row vector
  typedef dlib::linear_kernel<SampleType> StructSvmKernel;
};


template <typename StructSvmTrait>
class StructSvm : public Svm {

public:
  typedef typename StructSvmTrait::ScalarType ScalarType;
  typedef typename StructSvmTrait::SampleType SampleType;
  typedef typename StructSvmTrait::StructSvmKernel StructSvmKernel;

  typedef dlib::multiclass_linear_decision_function<StructSvmKernel, Svm::LabelIndexType> LinearStructSvm;
  typedef dlib::matrix<ScalarType,0,1> Weight;

  StructSvm();
  StructSvm(std::size_t feature_dimensions, const LabelIdxMap& label_idx_map);
  virtual ~StructSvm();

  virtual int32_t predict(const cv::Mat_<float>& features, std::vector<double>* values) const;

  template <typename T>
  int32_t predict(const cv::Mat_<T>& features, std::vector<double>* values) const;

  int32_t predict(const SampleType& features, std::vector<double>* values) const;

  using Svm::predict;

  inline const LinearStructSvm& svm() const;
  inline LinearStructSvm& mutable_svm();

protected:

private:
  LinearStructSvm _svm;

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Svm>(*this);
    ar & _svm;
  }
};


typedef StructSvm<SparseStructSvmTrait> SparseStructSvm;
typedef StructSvm<SparseStructSvmTraitFloat> SparseStructSvmFloat;

typedef StructSvm<DenseStructSvmTrait> DenseStructSvm;
typedef StructSvm<DenseStructSvmTraitFloat> DenseStructSvmFloat;

////////////////////////////////////////////////////////////////////////////////


template <typename T>
inline const typename StructSvm<T>::LinearStructSvm& StructSvm<T>::svm() const {
  return _svm;
}

template <typename T>
inline typename StructSvm<T>::LinearStructSvm& StructSvm<T>::mutable_svm() {
  return _svm;
}

template <typename T>
StructSvm<T>::StructSvm()
: Svm(0, LabelIdxMap()){

}

template <typename T>
StructSvm<T>::StructSvm(
    std::size_t feature_dimensions,
    const LabelIdxMap& label_idx_map)
: Svm(feature_dimensions, label_idx_map)
  {

}

template <typename T>
StructSvm<T>::~StructSvm(){

}

template <typename T>
/*virtual*/ int32_t StructSvm<T>::predict(const cv::Mat_<float>& features, std::vector<double>* values) const {
  SampleType svm_features;
  dlib_helpers::copy_to_sample(features, &svm_features);
  return predict(svm_features, values);

}

template <typename T>
int32_t StructSvm<T>::predict(const SampleType& sample, std::vector<double>* values_) const {

  std::vector<double>& values = *values_;

  values.resize(_svm.labels.size());

  // copied from dlib/svm/function.h as we need the decision values
  LabelIndexType best_idx = _svm.labels[0];
  ScalarType best_score = dlib::dot(dlib::rowm(_svm.weights, 0), sample) - _svm.b(0);
  values[best_idx] = best_score;

  for (unsigned long i = 1; i < _svm.labels.size(); ++i) {
    const int32_t label_idx = _svm.labels[i];
    const ScalarType predicted_score = dlib::dot(dlib::rowm(_svm.weights, i), sample) - _svm.b(i);
    values[label_idx] = predicted_score;
    if (predicted_score > best_score) {
      best_score = predicted_score;
      best_idx = label_idx;
    }
  }
  return best_idx;
}

template <typename T>
template <typename TT>
int StructSvm<T>::predict(
    const cv::Mat_<TT>& features,
    std::vector<double>* values) const {

  SampleType sample;
  dlib_helpers::copy_to_sample(features, &sample);
  return predict(sample, values);
}



} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SPARSE_STRUCT_SVM_H_ */
