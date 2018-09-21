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

#include "cpp/learning/common/image_sample.hpp"
#include "cpp/learning/common/sample_patches.hpp"
#include "cpp/vision/geometry_utils.hpp"

#include "cpp/learning/forest/forest.hpp"


using namespace std;
using namespace cv;
using namespace learning::forest;
using namespace learning::common;
using namespace body_pose::poselets;
using namespace vision::geometry_utils;

using learning::forest::utils::eval_mc_forest;


typedef pair<double, pair<int,Rect> > HardNeg;


DEFINE_string(config_file, "/home/mdantone/scratch/grid/poselets_lookbook/double/double_1/pixel_hard_reg_all/0/config.txt", "configuration file");
DEFINE_string(poselet_path, "/home/mdantone/scratch/grid/poselets_lookbook/poselets/double/1/0_0.poselet", "path to poselet");
DEFINE_bool(debug_mode, true, "debug mode");
DEFINE_bool(hard_neg, false, "use hard neg");

DEFINE_int32(i_tree, 1, "i_tree");


HOGDescriptor get_hog_descriptor(int width, int height) {
  int checked_width = (width/16) * 16;
  int checked_height = (height/16) * 16;
  CHECK_GT(checked_width, 0);
  CHECK_GT(height, 0);

  const cv::Size hog_win_size = cv::Size( checked_width, checked_height);
  const cv::Size hog_blockSize = cv::Size(16,16);
  const cv::Size hog_blockStride = cv::Size(8,8);
  const cv::Size hog_cellSize = cv::Size(8,8);
  const int hog_nbins = 9;

  LOG(INFO) << "hog desc, width: " << width <<" -> " << checked_width;
  LOG(INFO) << "hog desc, width: " << height <<" -> " << checked_height;

  HOGDescriptor hog_extractor(hog_win_size, hog_blockSize, hog_blockStride, hog_cellSize, hog_nbins);
  return hog_extractor;
}

void load_images(
    const vector<Annotation>& annotations,
    vector<int> features,
    vector<Image>& image_samples,
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

  // creating Image_samples
  create_image_sample_mt(images, features, image_samples );
  LOG(INFO) << image_samples.size() << " imagesamples created. ";
  CHECK_EQ(image_samples.size(), annotations.size());

}




void get_forground_map_ptr(const Image* sample,
        const Forest<PoseletSample>* forest,
        cv::Mat_<float>* forground_maps,
        const cv::Rect roi,
        int step_size = 1,
        bool blur = true,
        cv::Rect sliding_window = Rect(0,0,0,0)) {
  learning::forest::utils::get_forground_map(*sample, *forest, *forground_maps,
                                             roi, step_size, blur, sliding_window);
}

void get_forground_maps_mt(const vector<Image> sampels,
    const Forest<PoseletSample>& forest,
    vector<cv::Mat_<float> >& forground_maps,
    int step_size = 2,
    cv::Rect window_size = Rect(0,0,0,0)) {

  int num_threads = ::utils::system::get_available_logical_cpus();
  if(num_threads > 1) {
    boost::thread_pool::executor e(num_threads);
    for(int i=0; i < sampels.size(); i ++) {
      Rect roi(0,0,sampels[i].width(), sampels[i].height());
      e.submit(boost::bind( &get_forground_map_ptr, &sampels[i],
                            &forest, &forground_maps[i], roi,
                            step_size, true, window_size));
    }
    e.join_all();
  }else{

    for(int i=0; i < sampels.size(); i ++) {
      Rect roi(0,0,sampels[i].width(), sampels[i].height());
      learning::forest::utils::get_forground_map(sampels[i],
          forest, forground_maps[i], roi, step_size, true, window_size);
    }
  }
}

bool extract_hog( HOGDescriptor& hog_extractor, PoseletSample& sample  ){
  const Mat& img = sample.get_feature_channel(0);
  vector<Point> locations;
  locations.push_back(
      Point(sample.get_roi().x,
            sample.get_roi().y ) );

  vector<float> descriptors;
  hog_extractor.compute(img, descriptors, cv::Size(), cv::Size(), locations);
  cv::Mat_<float> descriptor_mat(descriptors, false);
  descriptor_mat = descriptor_mat.reshape(0, locations.size());
  sample.set_descriptor(descriptor_mat);
  return (countNonZero(descriptor_mat) > 0 );
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Hostname: " << ::utils::net::get_host_name();

  const std::string config_file   = FLAGS_config_file;
  const std::string poselet_path  = FLAGS_poselet_path;
  const bool debug_mode           = FLAGS_debug_mode;
  const int i_tree                = FLAGS_i_tree-1;
  const bool hard_neg             = ((i_tree != 0) & FLAGS_hard_neg);

  LOG(INFO) << "poselet_path: " << poselet_path;
  LOG(INFO) << "config_file: " << config_file;
  LOG(INFO) << "hard_neg mode: " << hard_neg;

  boost::mt19937 rng;
  rng.seed(i_tree +1);
  srand(i_tree +1);

  ForestParam param;
  CHECK(learning::forest::loadConfigFile(config_file, param));
  param.features.clear();
  param.features.push_back(0);

  Tree<PoseletSample>* tree = 0;
  char savePath[200];
  sprintf(savePath, "%s%03d.txt", param.tree_path.c_str(), i_tree);
  if(Tree<PoseletSample>::load_tree( savePath, &tree)){
    if( tree->isFinished() ) {
      LOG(INFO) << "finished tree loaded" << endl;
      LOG(INFO) << savePath << endl;
      exit(0);
    }else{
      LOG(INFO) << "unfinished tree loaded, keep growing" << endl;
    }
  }


  Poselet poselet;
  CHECK(load_poselet(poselet_path, poselet));
  LOG(INFO) << "poselet loaded: " << poselet.name;
  LOG(INFO) << "annotations loaded: " << poselet.annotations.size();
  LOG(INFO) << "n parts: " << poselet.part_indices.size();

  param.patch_width  = poselet.poselet_size.width;
  param.patch_height = poselet.poselet_size.height;


  LOG(INFO) << "patchsize: " <<  param.patch_width << " " << param.patch_height;

  std::random_shuffle(poselet.annotations.begin(), poselet.annotations.end());
  std::random_shuffle(poselet.neg_annotations.begin(), poselet.neg_annotations.end());

  vector<Annotation> annotations = poselet.annotations;
  int num_samples = MIN(param.num_samples_per_tree, annotations.size());
  annotations.resize(num_samples);


  // loading images into memory
  vector<Image> image_samples;
  vector<Mat> images;
  load_images(annotations, param.features, image_samples, images);

  // sample the poselets
  vector<PoseletSample> pos_samples;
  vector<PoseletSample> neg_samples;

  for(int i=0; i < image_samples.size(); i++) {

    const Annotation& ann = annotations[i];
    vector<Point> parts;
    for(int j=0; j < poselet.part_indices.size(); j++) {
      parts.push_back(ann.parts[poselet.part_indices[j] ]);
    }
    Rect img_bbox = Rect(0,0,images[i].cols,images[i].rows);
    Rect poselet_bbox = annotations[i].bbox;

    vector<Rect> pos_rects, neg_rects;
    sample_rectangles_around_roi(images[i],
                       poselet_bbox,
                       1,
                       &rng,
                       pos_rects,
                       1.0,
                       10000);

    if(!hard_neg) {
      sample_rectangles_outside_roi(images[i],
                         poselet_bbox,
                         param.num_patches_per_sample,
                         &rng,
                         neg_rects,
                         100000);
    }

    for(int j=0; j< pos_rects.size(); j++) {
      pos_samples.push_back( PoseletSample( &image_samples[i],
                         pos_rects[j],
                         img_bbox,
                         parts,
                         poselet.part_indices ) );
      if(debug_mode){
        pos_samples.back().show();
      }
    }

    for(int j=0; j< neg_rects.size(); j++) {
      neg_samples.push_back( PoseletSample( &image_samples[i],
                          neg_rects[j]) );
      if(debug_mode){
        neg_samples.back().show();
      }
    }

  }


  vector<Image> neg_image_samples;
  vector<Mat> neg_images;
  if(hard_neg) {


    LOG(INFO) << "sample hard neg.";
    Forest<PoseletSample> forest;
    CHECK(Forest<PoseletSample>::load_forest(config_file, &forest));
    cv::Rect poselet_size = poselet.poselet_size;


    vector<Mat_<float> > foreground_map(image_samples.size());
    int step_size = 2;
    get_forground_maps_mt(image_samples, forest, foreground_map,
        step_size, poselet_size);

    for(int i=0; i < foreground_map.size(); i++) {
      Rect rect_scores = Rect(0,0, foreground_map[i].cols, foreground_map[i].rows);
      Rect inter = intersect(rect_scores, annotations[i].bbox);
      float v = -(annotations[i].bbox.width*annotations[i].bbox.height);
      foreground_map[i](inter).setTo(Scalar(v));
    }

    LOG(INFO) << "start sampling.";
    vector<HardNeg> hard_negatives;
    learning::common::sample_hard_negatives(foreground_map,
        pos_samples.size()*param.num_patches_per_sample,
        poselet_size,  hard_negatives);


    vector<HardNeg> hard_negatives_additional;
    bool load_additional_images = true;
    if( load_additional_images ) {
      if(poselet.neg_annotations.size() > 0 ) {
        load_images(poselet.neg_annotations, param.features,
            neg_image_samples, neg_images);

        vector<Mat_<float> > foreground_map_(neg_image_samples.size());
        get_forground_maps_mt(neg_image_samples, forest, foreground_map_,
            step_size, poselet_size);

        if(false) {
          vector<Annotation> neg_ann = poselet.neg_annotations;
          calculate_box(neg_ann, poselet.part_indices );

          for(int i=0; i < foreground_map_.size(); i++) {
            Rect rect_scores = Rect(0,0, foreground_map_[i].cols, foreground_map_[i].rows);
            Rect inter = intersect(rect_scores, neg_ann[i].bbox);
            float v = -(neg_ann[i].bbox.width*neg_ann[i].bbox.height);
            foreground_map_[i](inter).setTo(Scalar(v));
          }
        }

        learning::common::sample_hard_negatives(foreground_map_,
            pos_samples.size()*param.num_patches_per_sample,
            poselet_size,  hard_negatives_additional);
      }
    }

    while(neg_samples.size() < pos_samples.size() &&
        (hard_negatives_additional.size() > 0 ||
         hard_negatives.size() > 0)) {

      bool use_additional = false;
      if( hard_negatives.size() == 0 )
        use_additional = true;

      if( hard_negatives.size() > 0 &&
          hard_negatives_additional.size() > 0 &&
          hard_negatives_additional.back().first > hard_negatives.back().first) {
        use_additional = true;
      }


      if( !use_additional ) {
        int i_img = hard_negatives.back().second.first;
        Rect bbox = hard_negatives.back().second.second;

        hard_negatives.pop_back();
        neg_samples.push_back( PoseletSample( &image_samples[i_img],  bbox) );
        if(debug_mode){
          neg_samples.back().show();
        }
      }else{

        int i_img = hard_negatives_additional.back().second.first;
        Rect bbox = hard_negatives_additional.back().second.second;
        hard_negatives_additional.pop_back();
        neg_samples.push_back( PoseletSample( &neg_image_samples[i_img],  bbox) );
        if(debug_mode){
          neg_samples.back().show();
        }
      }
    }

  }



  if(neg_samples.size()  < pos_samples.size() && poselet.neg_annotations.size() > 0 )
  {


    if(neg_images.size() == 0  ) {
      load_images(poselet.neg_annotations, param.features, neg_image_samples, neg_images);
    }

    Rect poselet_bbox = poselet.poselet_size;
    poselet_bbox.x = - poselet_bbox.width;
    poselet_bbox.y = - poselet_bbox.height;
    for(int i=0; i < neg_images.size(); i++) {
      vector<Rect> neg_rects;
      sample_rectangles_outside_roi(neg_images[i],
                         poselet_bbox,
                         param.num_patches_per_sample,
                         &rng,
                         neg_rects,
                         100000);



      for(int j=0; j< neg_rects.size(); j++) {
        neg_samples.push_back( PoseletSample( &neg_image_samples[i],
                            neg_rects[j]) );
        if(debug_mode){
          neg_samples.back().show();
        }

      }

      if(neg_samples.size() > pos_samples.size()) {
        break;
      }
    }

  }




  // extrac hog features
  LOG(INFO) << "extract hog features";
  HOGDescriptor hog_extractor = get_hog_descriptor(poselet.poselet_size.width,
                                                   poselet.poselet_size.height);


  // create pointers
  vector<PoseletSample*> samples_ptr;
  for(int i=0; i < pos_samples.size(); i++) {
    if( extract_hog(hog_extractor, pos_samples[i] ) ) {
      samples_ptr.push_back(&pos_samples[i]);
      CHECK_EQ( pos_samples[i].roi.width, param.patch_width);
      CHECK_EQ( pos_samples[i].roi.height, param.patch_height);
    }
  }
  for(int i=0; i < neg_samples.size(); i++) {
    if( extract_hog(hog_extractor, neg_samples[i] ) ) {
      samples_ptr.push_back(&neg_samples[i]);
      CHECK_EQ( neg_samples[i].roi.width, param.patch_width);
      CHECK_EQ( neg_samples[i].roi.height, param.patch_height);
    }
  }

  random_shuffle( samples_ptr.begin(), samples_ptr.end());
  LOG(INFO) << "Start Training with "<< samples_ptr.size() << " samples"<< endl;

  Timing jobTimer;
  jobTimer.start();
  if (tree) {
    LOG(INFO) << "keep growing" << endl;
    tree->grow(samples_ptr, jobTimer, &rng);
  } else {
    tree = new Tree<PoseletSample>(samples_ptr, param, &rng, savePath, jobTimer);
  }
  return 0;

}
