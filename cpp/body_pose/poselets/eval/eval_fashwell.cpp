/*
 * eval.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: mdantone
 */


#include <gflags/gflags.h>

#include <istream>
#include <cassert>
#include <opencv2/opencv.hpp>
#include <boost/progress.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>

#include "cpp/utils/timing.hpp"
#include "cpp/learning/forest/forest.hpp"

#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/learning/pictorial_structure/pair_inferenz.hpp"
#include "cpp/learning/pictorial_structure/inferenz.hpp"
#include "cpp/learning/pictorial_structure/utils.hpp"

#include "cpp/utils/thread_pool.hpp"
#include "cpp/body_pose/clustering/body_clustering.hpp"
#include "cpp/vision/features/feature_channels/feature_channel_factory.hpp"
#include "cpp/vision/image_utils.hpp"

#include "cpp/body_pose/poselets/eval/eval_utils.hpp"
#include "cpp/body_pose/features/feature_sample.hpp"
#include "cpp/vision/min_max_filter.hpp"

using namespace learning::forest;
using namespace bodypose::features;
namespace fs = boost::filesystem;
using namespace std;
using namespace cv;
using namespace boost::assign;


DEFINE_string(forest_path, "/scratch_net/giggo/mdantone/grid/poselets_fashwell/single/single_1/pixel_hard_neg_class", "folder wehere all the forests are stored");
DEFINE_string(img_index_file, "/scratch_net/giggo/mdantone/grid/poselets_fashwell/test_set.txt", "");
DEFINE_string(result_file_name, "test.txt", "");

DEFINE_string(experiment_name, "", "");
DEFINE_int32(n_mixtures_poselet, 1, "mixutres poselet ");
DEFINE_int32(n_poselet_types, 2, "n_poselet_types ");

DEFINE_int32(mixtures_ps, 1, "mixutres_ps");
DEFINE_bool(debug_mode, true, "debug mode");
DEFINE_bool(mode, false, "true = regression, false = classification/backpropagation");


// pictorial structure
DEFINE_int32(ps_num_rotations, 12, "num_rotations");
DEFINE_double(ps_weight, 0.0, "ps_weight");

void eval_single_image(Forest<FeatureSample>* forest,
                       learning::common::Image* image  ) {

  // eval image
  cv::vector<cv::Mat> voting_maps;
  cv::Mat foreground_map;
  learning::forest::utils::eval_mc_forest( *forest, *image,
      50, 2, voting_maps, foreground_map, false);

  Mat gray = image->get_feature_channel(0);
  image->del_feature_channels();
  image->add_feature_channel(gray);
  foreground_map.convertTo(foreground_map, CV_8UC1, 255);
  image->add_feature_channel(foreground_map);
  for(int i=0; i < voting_maps.size(); i++) {
    voting_maps[i].convertTo(voting_maps[i], CV_8UC1, 255*4);
    vision::MinMaxFilter::maxfilt(voting_maps[i], 5);
    image->add_feature_channel(voting_maps[i]);
  }
  CHECK_GT(image->num_feature_channels(), 0 );
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  const boost::filesystem::path forest_path  = FLAGS_forest_path;

  const string img_index_file = FLAGS_img_index_file;
  const string result_file_name = FLAGS_result_file_name;

  const int mixtures_ps       = FLAGS_mixtures_ps;
  const bool debug_mode       = FLAGS_debug_mode;
  const bool mode             = FLAGS_mode;

  const string experiment_name= FLAGS_experiment_name;
  const int n_mixtures_poselet  = FLAGS_n_mixtures_poselet;
  const int n_poselet_types       = FLAGS_n_poselet_types;

  LOG(INFO) << "result_file_name: " << result_file_name;
  LOG(INFO) << "mixtures_ps: " << mixtures_ps;
  LOG(INFO) << "debug_mode: " << debug_mode;

  vector<learning::forest::Forest<body_pose::poselets::PoseletSample> >  forests;
  vector<body_pose::poselets::Poselet> poselets;
  if(experiment_name != "") {
    vector<string> poselet_types;
    poselet_types += "single", "double", "tripple", "quadruple";
    string base = "/scratch_net/giggo/mdantone/grid/poselets_lookbook/";
    for(int i=0; i < n_poselet_types; i++) {

      string tmp( boost::str(boost::format("%1%%2%/%2%_%3%/%4%") % base % poselet_types[i] % n_mixtures_poselet % FLAGS_experiment_name ));
      LOG(INFO) << "forest_path: " << tmp;
      CHECK(load_poselets_forets(tmp, forests, poselets));
    }
  }else{
    LOG(INFO) << "forest_path: " << forest_path.string();
    CHECK(load_poselets_forets(forest_path.string(), forests, poselets));
  }




  // loading GT
  vector<Annotation> annotations;
  load_annotations(annotations, img_index_file);

  learning::forest::ForestParam param = forests[0].getParam();
  vision::features::feature_channels::FeatureChannelFactory fcf = vision::features::feature_channels::FeatureChannelFactory();


  // PS
  learning::ps::JointParameter joint_param;

  joint_param.joint_type = learning::ps::CLUSTER_GAUSS;
  joint_param.num_rotations =  FLAGS_ps_num_rotations;
  joint_param.weight_alpha = FLAGS_ps_weight;
  joint_param.use_weights = ( joint_param.weight_alpha > 0 ) ;
  joint_param.zero_sum_weights = false;
  vector<learning::ps::JointParameter> joints_param(13, joint_param);

  vector<learning::ps::Model> models;
  body_pose::clustering::ClusterMethod method = body_pose::clustering::GLOBAL_POSE;
  string cluster_path = "/scratch_net/giggo/mdantone/grid/poselets_fashwell/clusters/";
  learning::ps::get_body_model_mixtures("/scratch_net/giggo/mdantone/grid/poselets_fashwell/train_set.txt",
      method, mixtures_ps, cluster_path, models, joints_param);
  LOG(INFO) <<  models.size() << " models created.";

  // end PS
  Forest<FeatureSample> augment_forest;

  ofstream outFile;
  if(!debug_mode) {
    LOG(INFO) <<  "result_file_name: " << result_file_name << endl;
    outFile.open(result_file_name.c_str(), ios::out);
  }

  annotations.resize(500);
  boost::progress_display show_progress(annotations.size());
  for (int i = 0; i < static_cast<int>(annotations.size()); ++i, ++show_progress) {


    // load image
    Mat image_org = imread(annotations[i].url,1);
    CHECK(image_org.data);

    Mat image = image_org;
//    vision::image_utils::extend_image(image_org, 20, image);


    learning::common::Image sample;
    sample.init(image, param.features, &fcf, false, i);


    vector<Mat_<float> > voting_maps;
    eval_voting_poselets_mt(forests, poselets, sample, &voting_maps, 2, mode);

    vector<Point> minimas(annotations[i].parts.size(), Point(-1,-1));
    float score = learning::ps::inferenz_multiple(models, voting_maps, minimas, false, image);



    if(debug_mode) {
      for( int j=0; j < minimas.size(); j++){
        LOG(INFO) << j << ": " <<  minimas[j].x  << " " << minimas[j].y << " ";
      }

      if(false) {
        plot(image, minimas, "", 1 );
        imshow("image", image);
        for(int j=0; j < voting_maps.size(); j++ ) {
          Mat plot = voting_maps[j].clone();
//          normalize(voting_maps[j], plot, 0, 1, CV_MINMAX);
          string map_name( boost::str(boost::format("map_%1%") % j));
          imshow(map_name, plot);
//          imwrite(f_name, plot*255);
        }
        waitKey(0);
      }else{
        for(int j=0; j < voting_maps.size(); j++ ) {
        }
        plot(image, minimas);
      }
    }else{

      if( i < 100) {
        string exp_name = forest_path.parent_path().stem().string();
        string tmp( boost::str(boost::format("/home/mdantone/public_html/share/lookbook/ps/%1%_%2%_ps%3%_r%4%.jpg") % i % exp_name % mixtures_ps % joint_param.num_rotations ));
//        plot(image, minimas, tmp, 10 );
      }
      outFile << annotations[i].url << " 0 0 0 0 0 " << annotations[i].parts.size() << " ";
      assert( minimas.size() == annotations[i].parts.size() );
      for( int j=0; j < minimas.size(); j++){
        Point rescaled = minimas[j];
        outFile << rescaled.x  << " " << rescaled.y << " ";
      }
      outFile << "\n";
      outFile.flush();
    }


  }
  return 0;
}
