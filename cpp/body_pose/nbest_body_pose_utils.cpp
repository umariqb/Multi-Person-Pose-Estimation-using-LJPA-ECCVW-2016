#include "cpp/body_pose/nbest_body_pose_utils.hpp"

bool detect_nbest_multiscale(cv::Mat& img_org,
                            std::vector<body_pose::Pose>& poses,
                            vision::features::CNNFeatures& cnn_feat_extractor,
                            vector<string> feature_names,
                            vector<Model>& models,
                            learning::common::utils::PeakDetectionParams& pyramid_param,
                            int max_side_length,
                            body_pose::BodyPoseTypes pose_type,
                            learning::ps::NBestParam nbest_param)
{


    vector<Mat> pyramid;
    float initial_scale = vision::image_utils::build_image_pyramid(img_org,
        pyramid_param.scale_count,
        pyramid_param.scale_factor, pyramid,
        max_side_length);

    float min_score = 0;

    std::vector<body_pose::Pose> detected_poses, cleaned_poses;

    for(int i_scale =0; i_scale < pyramid_param.scale_count; i_scale++) {

      // get voting maps;
      vector<Mat_<float> > cnn_features;
      cnn_feat_extractor.extract(pyramid[i_scale], feature_names, cnn_features, true, true);
      cnn_features.erase(cnn_features.begin());

      for(unsigned int fIdx=0; fIdx<cnn_features.size(); fIdx++){
          cv::Mat tmp = cnn_features[fIdx];
          imshow("test", tmp);
          waitKey(0);
      }

      std::vector<body_pose::Pose> poses_for_each_scale;
      learning::ps::inferenz_nbest_max_decoder(models, cnn_features, poses_for_each_scale, pose_type, nbest_param);

      double scale = initial_scale*pow(pyramid_param.scale_factor, i_scale);
      for(unsigned int pose_id=0; pose_id<poses_for_each_scale.size(); pose_id++){
        for(unsigned int part_id=0; part_id<poses_for_each_scale[pose_id].parts.size(); part_id++){
          poses_for_each_scale[pose_id].parts[part_id].x /= scale;
          poses_for_each_scale[pose_id].parts[part_id].y /= scale;
        }
      }

      detected_poses.insert(detected_poses.end(), poses_for_each_scale.begin(), poses_for_each_scale.end());
    }

    // eliminate overlappig poses
    eliminate_overlapping_poses(detected_poses, cleaned_poses,0.05);

    //sort with respect to cost
    std::sort(cleaned_poses.begin(), cleaned_poses.end(), NBestInferenz::by_inferenz_score());

    int available_n = std::min((unsigned int)cleaned_poses.size(), nbest_param.num_nbest);
    poses.insert(poses.end(), cleaned_poses.begin(),
                        cleaned_poses.begin()+available_n);

}

