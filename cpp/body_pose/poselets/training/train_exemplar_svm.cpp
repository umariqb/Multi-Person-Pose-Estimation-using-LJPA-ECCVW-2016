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
DEFINE_int32(hard_neg_rounds, 1, "");

template< typename FirstType, typename SecondType >
struct PairComparator {
  bool operator()( const pair<FirstType, SecondType>& p1, const pair<FirstType, SecondType>& p2 ) const {
    return( p1.first > p2.first );
  }
};

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
  int num_samples = 250;//annotations.size();
  annotations.resize(num_samples);


  // loading images into memory
  vector<Mat> images;
  load_images(annotations, images);

  // sample the poselets


  Size win_size( max(32, poselet.poselet_size.width),
                 max(32, poselet.poselet_size.height));


  awesomeness::learning::svm::LibLinearSvmParameters svm_param;
  svm_param.C = hard_neg_rounds;

  awesomeness::vision::detector::HoGSVM hog_svm;

  awesomeness::vision::detector::HoGSVMTrainer hog_svm_trainer(win_size, svm_param);
  LOG(INFO) << poselet.poselet_size.width << " -> " << hog_svm_trainer.get_win_width();
  LOG(INFO) << poselet.poselet_size.height << " -> " << hog_svm_trainer.get_win_height();

  Rect pos_rect;
  for( int i_example = 0; i_example < 10; i_example ++) {
    for(int i=0; i < images.size(); i++) {

      const Annotation& ann = annotations[i];
      Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
      Rect poselet_bbox = ann.bbox;
      Point p(poselet_bbox.x + poselet_bbox.width/2,
              poselet_bbox.y + poselet_bbox.height/2);
      p.x -= hog_svm_trainer.get_win_width() / 2;
      p.y -= hog_svm_trainer.get_win_height() / 2;

      if(i_example == i ) {
        hog_svm_trainer.push(images[i], p, 1);
        pos_rect = Rect(p.x, p.y, hog_svm_trainer.get_win_width(), hog_svm_trainer.get_win_height() );

      }

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

      hog_svm_trainer.push(images[i], n_points, 0);

    }

    LOG(INFO) << "start training";
    hog_svm = hog_svm_trainer.train( );
    LOG(INFO) << "done.";

    if(debug_mode) {
      hog_svm.display_weights();
    }


    vector<pair<float, pair<int,Point> > > results;


    // test images
    for(int i=0; i < 15; i++) {
      const Annotation& ann = annotations[i];
      Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
      Rect poselet_bbox = ann.bbox;
      Point p(poselet_bbox.x + poselet_bbox.width/2,
              poselet_bbox.y + poselet_bbox.height/2);
      p.x -= hog_svm_trainer.get_win_width() / 2;
      p.y -= hog_svm_trainer.get_win_height() / 2;

      float score = hog_svm.predict(images[i], p );
      results.push_back( make_pair(score, make_pair(i,p)));

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

        score = hog_svm.predict(images[i], p_neg );
        results.push_back( make_pair(score, make_pair(i,p_neg)));
      }
    }

    std::sort(results.begin(), results.end(), PairComparator<double,pair<int,Point> >() );

    try {
      Mat pos_img = images[i_example].clone();
      cv::rectangle(pos_img, pos_rect, cv::Scalar(255, 0, 255, 0));
      for(int i=0; i < results.size(); i++) {
        LOG(INFO ) << i << " " << results[i].first;
        int i_img = results[i].second.first;
        Point p = results[i].second.second;


        Mat img = images[i_img].clone();
        cv::rectangle(img, Rect(p.x,p.y, hog_svm_trainer.get_win_width(), hog_svm_trainer.get_win_height()), cv::Scalar(255, 0, 255, 0));

        imshow("pos", pos_img);
        imshow("nn", img);

        waitKey(0);
      }
    }catch(...) {

    }
  }





  return 0;

}
