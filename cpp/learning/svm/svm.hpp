/*
 * svm.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__SVM_HPP_
#define AWESOMENESS__LEARNING__SVM__SVM_HPP_

#include <string>
#include <map>
#include <vector>

#include <opencv2/core/core.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace awesomeness {
namespace learning {
namespace svm {


class Svm {
public:
  typedef int32_t LabelIndexType;
  typedef std::map<std::string, LabelIndexType> LabelIdxMap;
  Svm();

  virtual ~Svm();

  int32_t predict(const cv::Mat_<float>& features) const;
  virtual int32_t predict(const cv::Mat_<float>& features, std::vector<double>* values) const = 0;

  inline std::size_t feature_dimension() const;

  inline int32_t num_classes() const;

  inline int32_t idx_for_label(const std::string& label) const;
  inline const LabelIdxMap& label_idx_map() const;

protected:
  Svm(
      std::size_t feature_dimensions,
      const LabelIdxMap& _label_idx_map
      );


private:
  LabelIdxMap _label_idx_map;
  uint32_t _num_classes;
  std::size_t _feature_dimensions;


  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & _feature_dimensions;
    ar & _label_idx_map;
    ar & _num_classes;
  }
};

////////////////////////////////////////////////////////////////////////////////
inline std::size_t Svm::feature_dimension() const {
  return _feature_dimensions;
}

inline int32_t Svm::idx_for_label(const std::string& label) const {
  return _label_idx_map.at(label);
}

inline const Svm::LabelIdxMap& Svm::label_idx_map() const {
  return _label_idx_map;
}

inline int32_t Svm::num_classes() const {
  return _num_classes;
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__SVM_HPP_ */
