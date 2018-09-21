/*
 * poselet_sample.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: mdantone
 */

#ifndef POSELET_SAMPLE_HPP_
#define POSELET_SAMPLE_HPP_

#include "cpp/learning/forest/part_sample.hpp"
#include "cpp/learning/forest/sample.hpp"
#include "cpp/vision/features/simple_feature.hpp"
#include "cpp/body_pose/poselets/poselets.hpp"
#include "cpp/learning/forest/exemplar_svm_split.hpp"
#include "cpp/learning/forest/svn_split.hpp"


namespace body_pose {
namespace poselets {

class PoseletSample : public learning::common::ImageSample,
                      public learning::forest::MultiPartSample {

public:
  typedef learning::forest::ThresholdSplit<vision::features::PixelComparisonFeature> Split;
//  typedef learning::forest::ThresholdSplit<vision::features::PixelValueFeatuers> Split;
//  typedef learning::forest::ThresholdSplit<vision::features::SimplePatchFeature> Split;
//  typedef learning::forest::SVNSplit<vision::features::SURFFeature> Split;
//  typedef learning::forest::ExemplarSVMSplit<vision::features::DummyFeature> Split;

  typedef learning::forest::MultiPartLeaf Leaf;


  PoseletSample(const learning::common::Image* patch,
                cv::Rect poselet_bbox,
                cv::Rect image_roi,
                const std::vector<cv::Point> parts,
                const std::vector<int> part_indices);

  PoseletSample(const learning::common::Image* patch,
                cv::Rect roi);

  PoseletSample() : MultiPartSample(false) {};
  void extract(const vision::features::SURFFeature* feature,
      std::vector<float>& desc)  const;


  const cv::Mat_<float>& get_descriptor() const {
      return desc_;
  }

  void set_descriptor(const cv::Mat_<float>& desc) {
    desc_ = desc.clone();
  }

  void show(int waik_key = 0) const;


  static void generateSplit(const std::vector<PoseletSample*>& data,
                            int patch_width,
                            int patch_height,
                            int min_child_size,
                            int split_mode,
                            int depth,
                            const std::vector<float> class_weights,
                            int split_id,
                            Split* split) {
    generate_split(data, patch_width, patch_height, min_child_size, split_mode, depth, class_weights, split_id, split);
  }
  static void generate_split(const std::vector<PoseletSample*>& data,
                            int patch_width,
                            int patch_height,
                            int min_child_size,
                            int split_mode,
                            int depth,
                            const std::vector<float> class_weights,
                            int split_id,
                            learning::forest::ThresholdSplit<vision::features::PixelComparisonFeature>* split);

  static void generate_split(const std::vector<PoseletSample*>& data,
                            int patch_width,
                            int patch_height,
                            int min_child_size,
                            int split_mode,
                            int depth,
                            const std::vector<float> class_weights,
                            int split_id,
                            learning::forest::ThresholdSplit<vision::features::PixelValueFeatuers>* split);

  static void generate_split(const std::vector<PoseletSample*>& data,
                            int patch_width,
                            int patch_height,
                            int min_child_size,
                            int split_mode,
                            int depth,
                            const std::vector<float> class_weights,
                            int split_id,
                            learning::forest::ThresholdSplit<vision::features::PatchComparisionFeature>* split);


  static void generate_split(const std::vector<PoseletSample*>& data,
                              int patch_width,
                              int patch_height,
                              int min_child_size,
                              int split_mode,
                              int depth,
                              const std::vector<float> class_weights,
                              int split_id,
                              learning::forest::ExemplarSVMSplit<vision::features::DummyFeature>* split);

  static void generate_split(const std::vector<PoseletSample*>& data,
                              int patch_width,
                              int patch_height,
                              int min_child_size,
                              int split_mode,
                              int depth,
                              const std::vector<float> class_weights,
                              int split_id,
                              learning::forest::SVNSplit<vision::features::SURFFeature>* split);


  static void create_leaf(Leaf& leaf,
                          const std::vector<PoseletSample*>& set,
                          const std::vector<float>& class_weights,
                          int leaf_id = 0);

  static std::vector<int> get_part_indices(const std::vector<PoseletSample*>& set) {
    for(int i=0; i < set.size(); i++) {
      if(set[i]->is_pos()) {
        return set[i]->get_part_indices();
      }
    }
    return std::vector<int>();
  }

  static void get_class_weights(std::vector<float>& class_weights,
      const std::vector<PoseletSample*>& set) {
    class_weights.resize(1);
    int size = count_pos_samples(set);

    if(set.size() == size) {
      class_weights[0] = 1;
    }else{
      class_weights[0] = size / static_cast<float>(set.size() - size);
    }
    LOG(INFO) << "pos samples: " << size  << std::endl;
    LOG(INFO) << "neg samples: " << set.size() - size <<  std::endl;
    LOG(INFO) << "positive to negative ratio: " << class_weights[0] << std::endl;
  }

  std::vector<int> get_part_indices() const {
    return _part_indices;
  }

private:
  std::vector<int> _part_indices;

  // offset to center;
  cv::Point_<int> offset;

  cv::Mat_<float> desc_;
};



} /* namespace poselets */
} /* namespace body_pose */
#endif /* POSELET_SAMPLE_HPP_ */
