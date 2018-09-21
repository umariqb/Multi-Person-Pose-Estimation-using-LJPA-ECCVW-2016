/*
 * train_utils.hpp
 *
 *  Created on: Jul 30, 2013
 *      Author: mdantone
 */

#ifndef TRAIN_UTILS_HPP_
#define TRAIN_UTILS_HPP_


#include <opencv2/opencv.hpp>
#include "cpp/body_pose/utils.hpp"
namespace body_pose {
namespace poselets {

void crop_images(std::vector<Annotation>& annotations,
                 const std::vector<cv::Mat>& images,
                 std::vector<cv::Mat>& cropped_images,
                 float scale_factor = 1.1);

void calculate_box(std::vector<Annotation>& annotations,
                 const std::vector<int>& part_indicies,
                 float scale_factor = 1.1,
                 int default_size = 32);

void calculate_poselet_bbox(std::vector<Annotation>& annotations,
                 const std::vector<int>& part_indicies,
                 float scale_factor = 1.1,
                 int default_size = 48);

} /* namespace poselets */
} /* namespace body_pose */
#endif /* TRAIN_UTILS_HPP_ */
