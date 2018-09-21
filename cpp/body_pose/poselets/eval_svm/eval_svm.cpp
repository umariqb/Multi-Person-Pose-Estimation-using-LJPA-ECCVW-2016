/*
 * eval.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: mdantone
 */


#include <gflags/gflags.h>

#include <opencv2/opencv.hpp>
#include <boost/progress.hpp>

#include "cpp/body_pose/utils.hpp"
#include "cpp/body_pose/poselets/poselets.hpp"
#include "cpp/body_pose/poselets/poselet_sample.hpp"
#include "cpp/body_pose/poselets/training/train_utils.hpp"

#include "cpp/utils/serialization/serialization.hpp"
#include "cpp/utils/serialization/opencv_serialization.hpp"
#include "cpp/utils/system_utils.hpp"
#include "cpp/utils/string_utils.hpp"
#include "cpp/utils/net_utils.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/file_utils.hpp"

#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/learning/pictorial_structure/pair_inferenz.hpp"
#include "cpp/learning/pictorial_structure/inferenz.hpp"
#include "cpp/learning/pictorial_structure/utils.hpp"

#include "cpp/vision/geometry_utils.hpp"
#include "cpp/vision/detector/hog_svm.hpp"
#include "cpp/learning/forest/param.hpp"
#include "cpp/learning/common/sample_patches.hpp"


using namespace std;
using namespace cv;

DEFINE_string(svm_path, "/scratch_net/giggo/mdantone/grid/poselets_leed/svm/single_1/hog/", "folder wehere all the forests are stored");
DEFINE_string(img_index_file, "/home/mdantone/scratch/data/leed_sport/index_test.txt", "");
DEFINE_string(result_file_name, "test.txt", "");

DEFINE_int32(mixtures_ps, 1, "mixutres_ps");
DEFINE_bool(debug_mode, true, "debug mode");


bool load_detectors(string folder, vector<awesomeness::vision::detector::HoGSVM>& detectors,
    vector<body_pose::poselets::Poselet>& poselets) {

  std::vector<boost::filesystem::path> paths;
  ::utils::fs::collect_directories(folder, &paths);

  for(int i=0; i < paths.size(); i++) {
    LOG(INFO) << i << "" << paths[i];
    string path = paths[i].string()+"/model.svm";

    awesomeness::vision::detector::HoGSVM hog_svm;
    CHECK(::utils::serialization::read_simple_binary_archive(path, hog_svm));
    detectors.push_back(hog_svm);

    // load poselet
    string poselet_path = paths[i].string()+"/p.poselet";
    body_pose::poselets::Poselet p;
    CHECK(load_poselet(poselet_path, p));
    poselets.push_back(p);
  }

  CHECK_EQ(detectors.size(), poselets.size());
  return (detectors.size() == poselets.size());
}

void eval_detectors(vector<awesomeness::vision::detector::HoGSVM>& detectors,
    vector<body_pose::poselets::Poselet> poselets,
    const Mat& image,
    vector<Mat_<float> >& maps) {

  vector<Mat_<float> > scores(detectors.size() );
  int num_threads = ::utils::system::get_available_logical_cpus();
  boost::thread_pool::executor e(num_threads);
  for(unsigned int i = 0; i < detectors.size(); i++) {
    e.submit(boost::bind( &awesomeness::vision::detector::HoGSVM::detect_ptr, &detectors[i], &image, &scores[i] ));
  }
  e.join_all();


  // allocate votingmaps
  Rect roi(0,0,image.cols,image.rows);
  maps.resize(13);
  for(int i=0; i < maps.size(); i++){
    maps[i] = cv::Mat::zeros(roi.height, roi.width, cv::DataType<float>::type);
  }

  // copy
  for(unsigned int i = 0; i < detectors.size(); i++) {
    int part_id = poselets[i].part_indices[0];
    cv::add(maps[part_id], scores[i], maps[part_id]);
  }

}


int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  const string svm_path    = FLAGS_svm_path;

  const string img_index_file = FLAGS_img_index_file;
  const string result_file_name = FLAGS_result_file_name;

  const int mixtures_ps       = FLAGS_mixtures_ps;
  const bool debug_mode       = FLAGS_debug_mode;

  LOG(INFO) << "result_file_name: " << result_file_name;
  LOG(INFO) << "mixtures_ps: " << mixtures_ps;
  LOG(INFO) << "debug_mode: " << debug_mode;

  LOG(INFO) << "path: " << svm_path;
  vector<awesomeness::vision::detector::HoGSVM> detectors;
  vector<body_pose::poselets::Poselet> poselets;
  CHECK(load_detectors(svm_path, detectors, poselets));

  // loading GT
  vector<Annotation> annotations;
  load_annotations(annotations, img_index_file);

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
  string cluster_path = "/scratch_net/giggo/mdantone/grid/pose_leed/clusters/";
  learning::ps::get_body_model_mixtures("/home/mdantone/scratch/data/leed_sport/index_all_all.txt",
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
    Mat image = imread(annotations[i].url,1);
    CHECK(image.data);

    vector<Mat_<float> > voting_maps;
    eval_detectors(detectors, poselets, image, voting_maps);

    vector<Point> minimas(annotations[i].parts.size(), Point(-1,-1));
    float score = learning::ps::inferenz_multiple(models,
                                                  voting_maps, minimas,
                                                  false, image);


    if(debug_mode) {
      for( int j=0; j < minimas.size(); j++){
        LOG(INFO) << j << ": " <<  minimas[j].x  << " " << minimas[j].y << " ";
      }
      plot(image, minimas);

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
