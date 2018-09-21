/*
 * detection.hpp
 *
 *  Created on: July 22, 2017
 *      Author: Umar Iqbal
 *  Re-Implementation of "Joint Subset Partition and Labeling
 *  for Multi Person Pose Estimation", ICCV-2015.
 */

#ifndef DETECTION_HH
#define DETECTION_HH

#include <opencv2/opencv.hpp>
#include <vector>


namespace learning
{

namespace splp
{

struct Detection
{
   Detection(cv::Point loc_ = cv::Point(0,0),
            std::vector<float> _conf_values = std::vector<float>()):
      label(-1), loc(loc_), conf_values(_conf_values), scale(0),
      score(0) {}

    int label;
    cv::Point loc;
    cv::Mat_<float> conf_values;
    float scale;
    float score;
};



}

}

#endif
