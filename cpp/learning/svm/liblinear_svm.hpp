/*
 * visual_svm.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_H_
#define AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_H_

#include <vector>

#include <boost/serialization/map.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include "cpp/utils/libsvm/liblinear.hpp"

#include "svm.hpp"

namespace awesomeness {
namespace learning {
namespace svm {


class LibLinearSvm : public Svm {

public:
  LibLinearSvm();

  typedef utils::liblinear::LinearHolder LinearSvm;


  LibLinearSvm(std::size_t feature_dimensions, const LabelIdxMap& label_idx_map);
  virtual ~LibLinearSvm();

  virtual int32_t predict(const cv::Mat_<float>& features, std::vector<double>* values) const;
  using Svm::predict;

  inline const LinearSvm& svm() const;
  inline LinearSvm& mutable_svm();

  void get_weights( std::vector<double>& weights) const;

protected:

private:
  LinearSvm _svm;

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Svm>(*this);
    ar & _svm;
    // if this fails, different vocab sizes were used
    CHECK_EQ(_svm.get_model().nr_feature, feature_dimension());
  }
};



////////////////////////////////////////////////////////////////////////////////
inline const LibLinearSvm::LinearSvm&  LibLinearSvm ::svm() const {
  return _svm;
}
inline LibLinearSvm::LinearSvm& LibLinearSvm ::mutable_svm() {
  return _svm;
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SIMPLE_SVM_H_ */
