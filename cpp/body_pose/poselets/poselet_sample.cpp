/*
 * poselet_sample.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: mdantone
 */

#include "poselet_sample.hpp"


using namespace std;
using namespace cv;

namespace body_pose {
namespace poselets {

struct IsPosSampleFunction {
  inline int operator()(const PoseletSample* s) const {
    if( s->is_pos()){
      return 1;
    }else{
      return 0;
    }
  }
};



PoseletSample::PoseletSample(const learning::common::Image* patch,
                Rect poselet_bbox,
                Rect image_roi,
                const std::vector<cv::Point> parts,
                const std::vector<int> part_indices)
    : learning::common::ImageSample(patch, poselet_bbox), MultiPartSample(true), _part_indices(part_indices){

  CHECK_GE(patch->width(),  (poselet_bbox.x+poselet_bbox.width) );
  CHECK_GE(patch->height(), (poselet_bbox.y+poselet_bbox.height) );
  CHECK_GE(poselet_bbox.x, 0);
  CHECK_GE(poselet_bbox.y, 0);

  Point center_image_roi(image_roi.width / 2, image_roi.height /2);
  Point center_poselet( poselet_bbox.x + poselet_bbox.width / 2,
                        poselet_bbox.y + poselet_bbox.height / 2);

  int num_parts = parts.size();
  part_offsets.resize(num_parts);
  for (int i = 0; i < num_parts; i++) {
    part_offsets[i].x = (parts[i].x - center_poselet.x);
    part_offsets[i].y = (parts[i].y - center_poselet.y);
  }


  offset.x = (center_image_roi.x - center_poselet.x);
  offset.y = (center_image_roi.y - center_poselet.y);

}

PoseletSample::PoseletSample(const learning::common::Image* image_, Rect roi_) :
    learning::common::ImageSample(image_, roi_), MultiPartSample(false) {

  CHECK_GE(image_->width(),  (roi_.x+roi_.width) );
  CHECK_GE(image_->height(), (roi_.y+roi_.height) );
  CHECK_GE(roi_.x, 0);
  CHECK_GE(roi_.y, 0);

}

void PoseletSample::show(int waik_key) const {

  cv::Mat img = get_feature_channel(0).clone();
  cv::imshow("sample", img(roi));

  Point center_poselet( roi.x + roi.width / 2,
                        roi.y + roi.height / 2);

  if(is_pos()) {


    Point offset_poselet = offset + center_poselet;
    cv::circle(img, offset_poselet, 3, cv::Scalar(255, 255, 255, 0), -1);

    for(int i=0; i < part_offsets.size(); i++) {
      Point offset_part = part_offsets[i] + center_poselet;
      cv::circle(img, offset_part, 3, cv::Scalar(255, 255, 255, 0), -1);
    }
  }

  cv::rectangle(img, roi, cv::Scalar(255, 255, 255, 0));
  cv::imshow("image", img);
  cv::waitKey(0);
}


void PoseletSample::generate_split(const std::vector<PoseletSample*>& data,
                          int patch_width,
                          int patch_height,
                          int min_child_size,
                          int split_mode,
                          int depth,
                          const std::vector<float> class_weights,
                          int split_id,
                          learning::forest::ThresholdSplit<vision::features::PixelComparisonFeature>* split) {

  boost::mt19937 rng(abs(split_id + 1) * (depth+1)* data.size());
  // generate the split
  int num_feat_channels = data[0]->get_images().size();
//
  int num_thresholds = 50;
  int margin = 0;
  split->initialize(&rng, patch_width, patch_height,
      num_feat_channels, num_thresholds,
      margin, depth);


  int sub_sampling = 10000;
  split->train(data,split_mode, depth, class_weights, min_child_size, &rng, sub_sampling);
}

void PoseletSample::generate_split(const std::vector<PoseletSample*>& data,
                          int patch_width,
                          int patch_height,
                          int min_child_size,
                          int split_mode,
                          int depth,
                          const std::vector<float> class_weights,
                          int split_id,
                          learning::forest::ThresholdSplit<vision::features::PixelValueFeatuers>* split) {

  boost::mt19937 rng(abs(split_id + 1) * (depth+1)* data.size());
  // generate the split
  int num_feat_channels = data[0]->get_images().size();
  CHECK_GT(num_feat_channels, 0);
  int num_thresholds = 50;
  int margin = 0;
  split->initialize(&rng, patch_width, patch_height,
      num_feat_channels, num_thresholds,
      margin, depth);


  int sub_sampling = 50000;
  split->train(data,split_mode, depth, class_weights, min_child_size, &rng, sub_sampling);
}


void PoseletSample::generate_split(const std::vector<PoseletSample*>& data,
                          int patch_width,
                          int patch_height,
                          int min_child_size,
                          int split_mode,
                          int depth,
                          const std::vector<float> class_weights,
                          int split_id,
                          learning::forest::ThresholdSplit<vision::features::PatchComparisionFeature>* split) {

  boost::mt19937 rng(abs(split_id + 1) * (depth+1)* data.size());
  // generate the split
  int num_feat_channels = data[0]->get_images().size();
//
  int num_thresholds = 50;
  int margin = 0;
  split->initialize(&rng, patch_width, patch_height,
      num_feat_channels, num_thresholds,
      margin, depth);


  int sub_sampling = 1000;
  split->train(data,split_mode, depth, class_weights, min_child_size, &rng, sub_sampling);
}

void PoseletSample::generate_split(const std::vector<PoseletSample*>& data,
                            int patch_width,
                            int patch_height,
                            int min_child_size,
                            int split_mode,
                            int depth,
                            const std::vector<float> class_weights,
                            int split_id,
                            learning::forest::ExemplarSVMSplit<vision::features::DummyFeature>* split) {

  int seed = abs(split_id + 1) * (depth+1)* data.size();
  boost::mt19937 rng(seed);

  vector<int> pos_sample_ids;
  vector<int> neg_sample_ids;
  for(int i=0; i < data.size(); i++) {
    if(data[i]->is_pos()) {
      pos_sample_ids.push_back(i);
    }else{
      neg_sample_ids.push_back(i);
    }
  }

  // add positive examples
  srand(seed);
  std::random_shuffle(pos_sample_ids.begin(), pos_sample_ids.end());
//  std::random_shuffle(neg_sample_ids.begin(), neg_sample_ids.end());
  pos_sample_ids.resize(1);
  int n_neg_samples = std::min( 10000, static_cast<int>(neg_sample_ids.size() ) );
  neg_sample_ids.resize(n_neg_samples);



  std::vector<int> weight_label(2);
  weight_label[0] = 0;
  weight_label[1] = 1;
  std::vector<double> weights(2);
  weights[0] = static_cast<double>(neg_sample_ids.size())
                                 / pos_sample_ids.size();
  weights[1] = 1.0;


  split->train(data, pos_sample_ids, neg_sample_ids, weight_label);
  split->find_threshold(data, 50, class_weights, split_mode, depth, min_child_size, 10000);
//  split->evaluate(data, class_weights, split_mode, depth, min_child_size);

}

void PoseletSample::extract(const vision::features::SURFFeature* feature,
    std::vector<float>& desc)  const {
  feature->extract( get_feature_channels(), get_roi(), desc);
}


void PoseletSample::generate_split(const std::vector<PoseletSample*>& data,
    int patch_width,
    int patch_height,
    int min_child_size,
    int split_mode,
    int depth,
    const std::vector<float> class_weights,
    int split_id,
    learning::forest::SVNSplit<vision::features::SURFFeature>* split) {

  int seed = abs(split_id + 1) * abs(depth+1)* data.size();
  boost::mt19937 rng(seed);
  // generate the split
  int num_feat_channels = data[0]->get_images().size();

  vision::features::SURFFeature f;
  f.generate(patch_width, patch_height, &rng, num_feat_channels);
  split->set_feature(f);

  ::utils::liblinear::solver_type::T solver = ::utils::liblinear::solver_type::L2R_L2LOSS_SVC;

  split->train(data, IsPosSampleFunction(), &rng, solver);
  split->evaluate(data, class_weights, split_mode, depth);

}

void PoseletSample::create_leaf(Leaf& leaf,
                        const std::vector<PoseletSample*>& set,
                        const std::vector<float>& class_weights,
                        int leaf_id ){
  bool print = (leaf.num_samples == 0);
  leaf.num_samples = set.size();

  int pos = count_pos_samples(set);
  float weight = 1.0;
  if ( class_weights.size() > 0 )
    weight = class_weights[0];

  double num_pos = static_cast<double>(pos);
  double num_neg = static_cast<double>(set.size() - pos);
  leaf.forground = num_pos / (num_neg*weight + num_pos );


  // offset to bbox-center
  leaf.offset = Point_<int>(0,0);
  for(int i=0; i < set.size(); i++) {
    const PoseletSample* s = set[i];
    if (s->is_pos() ) {
      leaf.offset.x += s->offset.x;
      leaf.offset.y += s->offset.y;
    }
  }
  if(pos > 0) {
    leaf.offset.x /= pos;
    leaf.offset.y /= pos;
  }
  leaf.variance = 0;


  if(print) {
    LOG(INFO) << "num_pos : " << pos <<  ", ratio : " << leaf.forground << ", samples: " << leaf.num_samples;

//    for(int i=0; i <set.size(); i++) {
//      if( set[i]->is_pos())
//        set[i]->show(0);
//    }
  }
  // mean offset to parts

  int n_pos = MultiPartSample::get_means(set, leaf.part_offsets);
  CHECK_EQ(num_pos, n_pos);
  CHECK_EQ(pos, n_pos);

  for(int i=0; i < leaf.part_offsets.size(); i++) {
    float ssd = MultiPartSample::sum_squared_difference(set, i, leaf.part_offsets[i]);
    leaf.part_variances.push_back( ssd );
  }
  leaf.part_indices = get_part_indices(set);

}

} /* namespace poselets */
} /* namespace body_pose */
