/*
 * vl_svm.hpp
 *
 *  Created on: Sep 26, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__VL_SVM_HPP_
#define AWESOMENESS__LEARNING__SVM__VL_SVM_HPP_

#include "svm.hpp"


namespace awesomeness {
namespace learning {
namespace svm {

class VlSvm: public virtual Svm {
public:
  VlSvm();
  VlSvm(std::size_t feature_dimensions, const LabelIdxMap& label_idx_map);
  virtual ~VlSvm();

  using Svm::predict;
  virtual int32_t predict(const cv::Mat_<float>& features, std::vector<double>* values) const;


  cv::Mat_<double>& mutable_w() { return _w;}
  const cv::Mat_<double>& w() const {return _w;}

  double b() const {return _b;}
  void set_b(double b) { _b = b;}

private:
  cv::Mat_<double> _w;
  double _b;


  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Svm>(*this);
    ar & _w;
    ar & _b;
  }
};

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__VL_SVM_HPP_ */
