/*
 * utils.hpp
 *
 *  Created on: July 22, 2017
 *      Author: Umar Iqbal
 *  Re-Implementation of "Joint Subset Partition and Labeling
 *  for Multi Person Pose Estimation", ICCV-2015.
 */


#ifndef UTILS_HH
#define UTILS_HH

#include <opencv2/opencv.hpp>
#include "cpp/learning/SPLPI/detection.hpp"
#include "cpp/body_pose/body_pose_types.hpp"

namespace learning
{

namespace splpi
{

cv::Mat compute_spatial_relation_feature(const learning::splpi::Detection& d1, const learning::splpi::Detection& d2, float patch_size);
cv::Mat compute_spatial_relation_feature(const cv::Point pt1, const cv::Point pt2, float patch_size);
cv::Mat shuffleRows(const cv::Mat &matrix);
cv::Mat extract_pairwise_feat(cv::Point& p1, cv::Point& p2);
bool extract_pairwise_feat_from_offsets(std::vector<cv::Point> offsets, cv::Mat& feat);
bool extract_pairwise_feat_from_offset(cv::Point offset, cv::Mat& feat);
void normalize_data_range(const cv::Mat& data, cv::Mat& norm_data, cv::Mat& means, cv::Mat& stds);
void normalize_data(cv::Mat data, cv::Mat dst,  cv::Mat mean, cv::Mat std);

void draw_detections(cv::Mat image, std::vector<learning::splpi::Detection>& detections, bool draw_lines);

void nms(std::vector<learning::splpi::Detection>& src,
          std::vector<learning::splpi::Detection>& dst,
          float thresh);

void group_detections(std::vector<learning::splpi::Detection>& src,
                       std::vector<learning::splpi::Detection>& dst,
                       float thresh = 0);

}

}

#endif
