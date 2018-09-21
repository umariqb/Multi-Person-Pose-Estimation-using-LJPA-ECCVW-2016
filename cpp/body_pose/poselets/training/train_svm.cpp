/*
 * train.cpp
 *
 *  Created on: Jul 30, 2013
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
#include "cpp/utils/net_utils.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/system_utils.hpp"
#include "cpp/utils/string_utils.hpp"


#include "cpp/learning/common/sample_patches.hpp"
#include "cpp/vision/geometry_utils.hpp"
#include "cpp/vision/detector/hog_svm.hpp"
#include "cpp/learning/forest/param.hpp"


using namespace std;
using namespace cv;
using learning::forest::ForestParam;

typedef pair<double, pair<int,Rect> > HardNeg;


DEFINE_string(config_file, "/home/mdantone/scratch/grid/poselets_leed/test/pixel/0/config.txt", "configuration file");
DEFINE_string(poselet_path, "/home/mdantone/scratch/grid/poselets_leed/poselets/single/1/0_0.poselet", "path to poselet");
DEFINE_bool(debug_mode, true, "debug mode");
DEFINE_int32(hard_neg_rounds, 0, "");

void load_images(
    const vector<Annotation>& annotations,
    vector<Mat>& images) {

  // loading neg images into memory
  vector<boost::filesystem::path> image_paths;
  for(int i=0; i < annotations.size(); i ++) {
    image_paths.push_back(annotations[i].url);
  }
  LOG(INFO) << image_paths.size() << " images found";
  ::utils::image_file::load_images(image_paths, images);
  CHECK_EQ(image_paths.size(),images.size());
  LOG(INFO) << "and loaded.";

}



int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Hostname: " << ::utils::net::get_host_name();

  const std::string config_file   = FLAGS_config_file;
  const std::string poselet_path  = FLAGS_poselet_path;
  const int hard_neg_rounds       = FLAGS_hard_neg_rounds;

  const bool debug_mode           = FLAGS_debug_mode;

  LOG(INFO) << "poselet_path: " << poselet_path;
  LOG(INFO) << "config_file: " << config_file;

  boost::mt19937 rng;
  ForestParam param;
  CHECK(learning::forest::loadConfigFile(config_file, param));

  body_pose::poselets::Poselet poselet;
  CHECK(body_pose::poselets::load_poselet(poselet_path, poselet));
  LOG(INFO) << "poselet loaded: " << poselet.name;
  LOG(INFO) << "annotations loaded: " << poselet.annotations.size();
  LOG(INFO) << "n parts: " << poselet.part_indices.size();

  param.patch_width = poselet.poselet_size.width;
  param.patch_height = poselet.poselet_size.height;
  LOG(INFO) << "patchsize: " <<  param.patch_width << " " << param.patch_height;

  std::random_shuffle(poselet.annotations.begin(), poselet.annotations.end());
  std::random_shuffle(poselet.neg_annotations.begin(), poselet.neg_annotations.end());

  vector<Annotation> annotations = poselet.annotations;
  int num_samples = annotations.size();
  annotations.resize(num_samples);


  // loading images into memory
  vector<Mat> images;
  load_images(annotations, images);

  // sample the poselets


  Size win_size( max(32, poselet.poselet_size.width),
                 max(32, poselet.poselet_size.height));


  awesomeness::learning::svm::LibLinearSvmParameters svm_param;
  svm_param.solver_type = (utils::liblinear::solver_type::T) hard_neg_rounds;

  awesomeness::vision::detector::HoGSVM hog_svm;
  bool hard_neg = true;
  if( !hard_neg ) {

    awesomeness::vision::detector::HoGSVMTrainer hog_svm_trainer(win_size, svm_param);
    LOG(INFO) << poselet.poselet_size.width << " -> " << hog_svm_trainer.get_win_width();
    LOG(INFO) << poselet.poselet_size.height << " -> " << hog_svm_trainer.get_win_height();

    for(int i=0; i < images.size(); i++) {

      const Annotation& ann = annotations[i];

      Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
      Rect poselet_bbox = ann.bbox;
      Point p(poselet_bbox.x + poselet_bbox.width/2,
              poselet_bbox.y + poselet_bbox.height/2);
      p.x -= hog_svm_trainer.get_win_width() / 2;
      p.y -= hog_svm_trainer.get_win_height() / 2;
      hog_svm_trainer.push(images[i], p, 1);


      vector<Rect>  neg_rects;
      learning::common::sample_rectangles_outside_roi(images[i],
                           poselet_bbox,
                           1,
                           &rng,
                           neg_rects,
                           100000);

      vector<Point> n_points;
      for(int j=0; j < neg_rects.size(); j++) {
        Point p_neg(neg_rects[j].x + poselet_bbox.width/2,
                neg_rects[j].y + poselet_bbox.height/2);
        p_neg.x -= hog_svm_trainer.get_win_width() / 2;
        p_neg.y -= hog_svm_trainer.get_win_height() / 2;
        n_points.push_back( p_neg );
      }

      hog_svm_trainer.push(images[i], n_points, 5);

    }

    LOG(INFO) << "start training";
    hog_svm = hog_svm_trainer.train();
    LOG(INFO) << "done.";

  }else{

    awesomeness::vision::detector::HoGSVMBootstrap hog_svm_trainer(win_size, svm_param);
    LOG(INFO) << poselet.poselet_size.width << " -> " << hog_svm_trainer.get_win_width();
    LOG(INFO) << poselet.poselet_size.height << " -> " << hog_svm_trainer.get_win_height();
    vector<Rect> rois;
    for(int i=0; i < images.size(); i++) {
      const Annotation& ann = annotations[i];
      Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
      Rect poselet_bbox = ann.bbox;
      Point p(poselet_bbox.x + poselet_bbox.width/2,
              poselet_bbox.y + poselet_bbox.height/2);
      p.x -= hog_svm_trainer.get_win_width() / 2;
      p.y -= hog_svm_trainer.get_win_height() / 2;

      rois.push_back( Rect(p.x, p.y,hog_svm_trainer.get_win_width(), hog_svm_trainer.get_win_height()));

    }


    hog_svm = hog_svm_trainer.bootstrap(images, rois, 5);
  }




  if(debug_mode) {
    hog_svm.display_weights();
  }


  boost::filesystem::path path(config_file);
  boost::filesystem::path p_path(poselet_path);

//  string f_name(boost::str(boost::format("%1%/%2%.svm" ) % path.parent_path().string() % p_path.stem().string() ));
  string f_name(boost::str(boost::format("%1%/model.svm" ) % path.parent_path().string() ));

  LOG(INFO) << "saving model to: " << f_name;

  CHECK(::utils::serialization::write_simple_binary_archive(f_name, hog_svm ));
  LOG(INFO) << "saved.";


//  for(int i=0;i < annotations.size(); i++) {
//    Mat_<float> score;
//    hog_svm.detect(images[i], score);
//  }

//  LOG(INFO) << "saving model to: " << f_name;
//  HoGSVM hog_svm2;
//  CHECK(::utils::serialization::read_simple_binary_archive(f_name, hog_svm2 ));
//
//
//  for(int i=0; i < 10; i++) {
//
//    const Annotation& ann = annotations[i];
//
//
//    Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
//    Rect poselet_bbox = annotations[i].bbox;
//    Point p(poselet_bbox.x + poselet_bbox.width/2,
//            poselet_bbox.y + poselet_bbox.height/2);
//    p.x -= hog_svm.get_win_width() / 2;
//    p.y -= hog_svm.get_win_height() / 2;
//    LOG(INFO) << 1 << " : " << hog_svm.predict(images[i], p);
//    LOG(INFO) << 1 << " : " << hog_svm2.predict(images[i], p);
//
//
//    vector<Rect>  neg_rects;
//    sample_rectangles_outside_roi(images[i],
//                         poselet_bbox,
//                         3,
//                         &rng,
//                         neg_rects,
//                         100000);
//
//    vector<Point> n_points;
//    for(int j=0; j < neg_rects.size(); j++) {
//      Point p_neg(neg_rects[j].x + poselet_bbox.width/2,
//              neg_rects[j].y + poselet_bbox.height/2);
//      p_neg.x -= hog_svm.get_win_width() / 2;
//      p_neg.y -= hog_svm.get_win_height() / 2;
//      n_points.push_back( p_neg );
//    }
//
//    for(int j=0; j < neg_rects.size(); j++) {
//      LOG(INFO) << 0 << " : " << hog_svm.predict(images[i], n_points[j]);
//      LOG(INFO) << 0 << " : " << hog_svm2.predict(images[i], n_points[j]);
//
//    }
//
//
//    Mat img = images[i].clone();
//    cv::rectangle(img, Rect(p.x, p.y, hog_svm.get_win_width(), hog_svm.get_win_height()),
//        cv::Scalar(255, 0, 255, 0));
//
//    for(int j=0; j < n_points.size(); j++) {
//      cv::rectangle(img, Rect(n_points[j].x, n_points[j].y, hog_svm.get_win_width(), hog_svm.get_win_height()),
//          cv::Scalar(255, 255, 255, 0));
//    }
//    imshow("X", img);
//    waitKey(0);
//
//  }


  return 0;

}
