/*
 * learn_model_parameter.hpp
 *
 *  Created on: July 22, 2017
 *      Author: Umar Iqbal
 *  Re-Implementation of "Joint Subset Partition and Labeling
 *  for Multi Person Pose Estimation", ICCV-2015.
 */


#ifndef LEARN_MODEL_PARAMETER_HPP
#define LEARN_MODEL_PARAMETER_HPP

#include "cpp/learning/SPLP/splp.hpp"
#include "cpp/body_pose/body_pose_types.hpp"
#include "cpp/body_pose/common.hpp"
#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/vision/image_utils.hpp"
#include "cpp/vision/features/cnn/cnn_features.hpp"
#include "cpp/learning/forest/forest.hpp"
#include "cpp/body_pose/body_part_sample.hpp"


namespace learning
{

namespace splp
{

bool learn_model_parameters(SPLP& splp_model, std::string train_file, int patch_size, learning::ps::JointParameter params, body_pose::BodyPoseTypes pose_type);
bool learn_param_for_same_class_binaries(vector<Annotation>& annotations, SPLP& splp_model, int patch_size, body_pose::BodyPoseTypes pose_type);
bool learn_param_for_diff_class_binaries(vector<Annotation>& annotations, SPLP& splp_model, int patch_size, learning::ps::JointParameter params, body_pose::BodyPoseTypes pose_type);

bool learn_model_parameters_mp(SPLP& splp_model, std::string train_file,
                                int patch_size,
                                learning::ps::JointParameter params,
                                float scale_factor,
                                body_pose::BodyPoseTypes pose_type,
                                string cache);


bool learn_param_for_same_class_binaries_mp(vector<MultiAnnotation>& annotations,
                                            SPLP& splp_model, int patch_size,
                                            body_pose::BodyPoseTypes pose_type,
                                            string cache);

bool learn_param_for_diff_class_binaries_mp(vector<MultiAnnotation>& annotations,
                                            vector<vector<Mat> >& appearance_feats,
                                            SPLP& splp_model,
                                            learning::ps::JointParameter params,
                                            body_pose::BodyPoseTypes pose_type,
                                            string cache,
                                            int num_threads = -1
);
}

}

#endif // LEARN_MODEL_PARAMETER_HPP
