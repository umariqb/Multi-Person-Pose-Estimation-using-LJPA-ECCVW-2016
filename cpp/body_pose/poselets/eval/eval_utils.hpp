/*
 * eval_utils.hpp
 *
 *  Created on: Aug 2, 2013
 *      Author: mdantone
 */

#ifndef EVAL_UTILS_HPP_
#define EVAL_UTILS_HPP_

#include "cpp/learning/forest/forest.hpp"
#include "cpp/body_pose/poselets/poselet_sample.hpp"


namespace body_pose {
namespace poselets {

bool load_poselets_forets(std::string folder,
    std::vector<learning::forest::Forest<body_pose::poselets::PoseletSample> >& forests,
    std::vector<body_pose::poselets::Poselet>& poselets);

void eval_voting_poselets_mt(
    std::vector<learning::forest::Forest<body_pose::poselets::PoseletSample> >& forests,
    std::vector<body_pose::poselets::Poselet>& poselets,
    const learning::common::Image& image,
    std::vector<cv::Mat_<float> >* voting_maps,
    int step_size = 2,
    bool regression = true);

void eval_voting_poselets(
    std::vector<learning::forest::Forest<body_pose::poselets::PoseletSample> >* forests,
    std::vector<body_pose::poselets::Poselet>* poselets,
    const learning::common::Image* image,
    std::vector<cv::Mat_<float> >* voting_maps,
    int step_size = 2,
    bool regression = true);

cv::HOGDescriptor get_hog_descriptor(int width, int height);

} /* namespace poselets */
} /* namespace body_pose */
#endif /* EVAL_UTILS_HPP_ */
