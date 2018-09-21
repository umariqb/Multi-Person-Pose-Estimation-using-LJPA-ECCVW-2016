/*
 * poselets.hpp
 *
 *  Created on: Jul 30, 2013
 *      Author: mdantone
 */

#ifndef POSELETS_HPP_
#define POSELETS_HPP_

#include "cpp/utils/serialization/serialization.hpp"
#include "cpp/utils/serialization/opencv_serialization.hpp"
#include <opencv2/opencv.hpp>
#include "cpp/body_pose/common.hpp"

namespace body_pose {
namespace poselets {

class Poselet {
public:
  std::string name;
  std::vector<int> part_indices;
  cv::Rect poselet_size;
  std::vector<cv::Point_<int> > part_offsets;
  std::vector<float> part_variance;
  std::vector<Annotation> annotations;
  std::vector<Annotation> neg_annotations;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & name;
    ar & part_indices;
    ar & poselet_size;
    ar & part_offsets;
    ar & part_variance;
    ar & annotations;
    ar & neg_annotations;
  }
};

bool save_poselet(const Poselet& poselet, const std::string save_path);

bool load_poselet(const std::string save_path, Poselet& poselet);

}
}

#endif /* POSELETS_HPP_ */
