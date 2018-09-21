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



using namespace std;
using namespace cv;
using namespace boost::assign;


DEFINE_string(forest_path, "/scratch_net/giggo/mdantone/grid/poselets_lookbook/single/single_1/pixel_hard_reg_all", "folder wehere all the forests are stored");
DEFINE_string(img_index_file, "/scratch_net/giggo/mdantone/grid/poselets_amazon/fullbody_index.txt", "");
DEFINE_string(result_file_name, "test.txt", "");

DEFINE_int32(mixtures_ps, 1, "mixutres_ps");
DEFINE_bool(debug_mode, true, "debug mode");
DEFINE_bool(mode, true, "true = regression, false = classification/backpropagation");


int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  const string forest_path    = FLAGS_forest_path;

  const string img_index_file = FLAGS_img_index_file;
  const string result_file_name = FLAGS_result_file_name;

  const int mixtures_ps       = FLAGS_mixtures_ps;
  const bool debug_mode       = FLAGS_debug_mode;
  const bool mode             = FLAGS_mode;


  LOG(INFO) << "result_file_name: " << result_file_name;
  LOG(INFO) << "mixtures_ps: " << mixtures_ps;
  LOG(INFO) << "debug_mode: " << debug_mode;

  vector<learning::forest::Forest<body_pose::poselets::PoseletSample> >  forests;
  vector<body_pose::poselets::Poselet> poselets;
  LOG(INFO) << "forest_path: " << forest_path;
  CHECK(load_poselets_forets(forest_path, forests, poselets));





  // loading GT
  vector<Annotation> annotations;
  load_annotations(annotations, img_index_file);

  learning::forest::ForestParam param = forests[0].getParam();
  vision::features::feature_channels::FeatureChannelFactory fcf = vision::features::feature_channels::FeatureChannelFactory();


  // PS
  learning::ps::JointParameter joint_param;

  joint_param.joint_type = learning::ps::CLUSTER_GAUSS;
  joint_param.num_rotations =  24;//boost::lexical_cast<int>(argv[4]);
  joint_param.use_weights = true ;
  joint_param.weight_alpha = 0.1;//boost::lexical_cast<float>(argv[3]);
  joint_param.zero_sum_weights = false;
  vector<learning::ps::JointParameter> joints_param(13, joint_param);

  vector<learning::ps::Model> models;
  body_pose::clustering::ClusterMethod method = body_pose::clustering::GLOBAL_POSE;
  string cluster_path = "/scratch_net/giggo/mdantone/grid/pose_lookbook/clusters/";
  learning::ps::get_body_model_mixtures("/srv/glusterfs/mdantone/data/lookbook/index_rescaled_train_clean.txt",
      method, mixtures_ps, cluster_path, models, joints_param);
  LOG(INFO) <<  "model created";

  // end PS


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

    //resize img

    Mat image;
    int img_height = 250;
    float scale_factor =  static_cast<float>(img_height)/ image_org.rows;
    cv::resize(image_org, image, Size(image_org.cols*scale_factor, image_org.rows*scale_factor) );
//    vision::image_utils::extend_image(image_org, 20, image);


    learning::common::Image sample(image, param.features, fcf, false, i);

    vector<Mat_<float> > voting_maps;
    eval_voting_poselets_mt(forests, poselets, sample, &voting_maps, 2, mode);

    vector<Point> minimas(annotations[i].parts.size(), Point(-1,-1));
    float score = learning::ps::inferenz_multiple(models, voting_maps, minimas, false, image);


    if(debug_mode) {
      for( int j=0; j < minimas.size(); j++){
        LOG(INFO) << j << ": " <<  minimas[j].x  << " " << minimas[j].y << " ";
      }

      string tmp( boost::str(boost::format("/home/mdantone/public_html/share/lookbook/poselets/pixel_hard/%1%_%2%_ps%3%") % i % "double1" % mixtures_ps));

      LOG(INFO) << tmp;

//      plot(image, minimas, tmp+".jpg", 10 );
      plot(image, minimas );
      if(true) {
        imshow("image", image);
        for(int j=0; j < voting_maps.size(); j++ ) {
          Mat plot = voting_maps[j].clone();
          normalize(voting_maps[j], plot, 0, 1, CV_MINMAX);
          imshow("voting map", plot);
          string f_name( boost::str(boost::format("%1%_%2%.jpg") % tmp % j));
//          imwrite(f_name, plot*255);
          waitKey(0);
        }
      }
    }else{
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
