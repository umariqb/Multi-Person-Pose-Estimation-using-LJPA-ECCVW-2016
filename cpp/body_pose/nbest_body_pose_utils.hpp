#ifndef NBEST_BODY_POSE_UTILS_H
#define NBEST_BODY_POSE_UTILS_H

#include <istream>
#include <cassert>
#include <opencv2/opencv.hpp>
#include <boost/progress.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>

#include "cpp/utils/timing.hpp"
#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/learning/pictorial_structure/pair_inferenz.hpp"
#include "cpp/learning/pictorial_structure/nbest_inferenz.hpp"

#include "cpp/learning/pictorial_structure/utils.hpp"

#include "cpp/body_pose/utils.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/body_pose/body_pose_types.hpp"
#include "cpp/vision/image_utils.hpp"

#include "cpp/utils/pyramid_stitcher/pyramid_stitcher.hpp"
#include "cpp/vision/features/cnn/cnn_features.hpp"


using namespace boost::assign;
using namespace std;
using namespace cv;
using namespace learning::forest::utils;
using namespace learning::forest;
using namespace learning::ps;

bool detect_nbest_multiscale(cv::Mat& img_org,
                            std::vector<body_pose::Pose>& poses,
                            vision::features::CNNFeatures& cnn_feat_extractor,
                            std::vector<std::string> feature_names,
                            std::vector<Model>& models,
                            learning::common::utils::PeakDetectionParams& pyramid_param,
                            int max_side_length,
                            body_pose::BodyPoseTypes pose_type,
                            learning::ps::NBestParam nbest_param);

#endif // NBEST_BODY_POSE_UTILS_H
