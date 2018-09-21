/*
 * eval_utils.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: mdantone
 */

#include "eval_utils.hpp"
#include "cpp/utils/file_utils.hpp"
#include "cpp/body_pose/utils.hpp"
#include "cpp/learning/forest/forest.hpp"
#include "cpp/utils/system_utils.hpp"
#include "cpp/body_pose/poselets/poselets.hpp"

using namespace cv;
using namespace std;

namespace body_pose {
namespace poselets {

namespace bfs = boost::filesystem;

bool load_poselets_forets(string folder, vector<learning::forest::Forest<PoseletSample> >& forests, vector<Poselet>& poselets) {

  std::vector<bfs::path> paths;
  ::utils::fs::collect_directories(folder, &paths);

  int count_trees = 0;
  for(int i=0; i<paths.size(); i++) {
    learning::forest::ForestParam param_part;
    learning::forest::Forest<PoseletSample> forest;

    string config_file_path = paths[i].string()+"/config.txt";
    LOG(INFO) << config_file_path;

    CHECK(loadConfigFile(config_file_path, param_part));

    if(forest.load(param_part.tree_path, param_part) ){
      forests.push_back(forest);
      count_trees += forest.trees.size();


      // load poselet
      string poselet_path = paths[i].string()+"/p.poselet";
      Poselet p;
      CHECK(load_poselet(poselet_path, p));
      poselets.push_back(p);
    }
  }

  CHECK_EQ(forests.size(), poselets.size());


  LOG(INFO) << forests.size() << " of " << paths.size() << " forests loaded.";
  LOG(INFO) << count_trees << " total trees.";
  return forests.size() > 0;
}

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


  HOGDescriptor hog_extractor(hog_win_size, hog_blockSize, hog_blockStride, hog_cellSize, hog_nbins);
  return hog_extractor;
}

void eval_voting_hog(learning::forest::Forest<PoseletSample>& forest,
    Poselet& poselets,
    const learning::common::Image& sample,
    vector<Mat_<float> >* voting_maps,
    bool regression = true) {

  if(forest.trees.size() == 0)
    return;
  vector<Mat_<float> >& maps = *voting_maps;
  Rect roi(0,0,sample.width(),sample.height());
  int step_size = 2;
  learning::forest::ForestParam param = forest.getParam();
  vector<PoseletSample> sampels;
  vector<Point> locations;
  int patch_width = param.patch_width;
  int patch_height = param.patch_height;
  for (int x = roi.x; x < roi.x + roi.width - patch_width; x += step_size) {
    for (int y = roi.y; y < roi.y + roi.height - patch_height; y += step_size) {
      cv::Rect patch_box(x, y, patch_width, patch_height);
      locations.push_back(Point(x,y));
      sampels.push_back( PoseletSample(&sample, patch_box) );
    }
  }

  if(locations.size() == 0) {
    return;
  }

  HOGDescriptor hog_extractor = get_hog_descriptor(patch_width, patch_height);
  vector<float> descriptors;
  hog_extractor.compute(sample.get_feature_channel(0),
                        descriptors,
                        cv::Size(), cv::Size(), locations);
  cv::Mat_<float> descriptor_mat(descriptors, false);
  descriptor_mat = descriptor_mat.reshape(0, locations.size());
  CHECK_EQ(descriptor_mat.cols, hog_extractor.getDescriptorSize());
  CHECK_EQ(descriptor_mat.rows, locations.size());


  for(int i_sample=0; i_sample < sampels.size(); i_sample++) {
    sampels[i_sample].set_descriptor( descriptor_mat.row(i_sample) );
  }

  for(int i_sample=0; i_sample < sampels.size(); i_sample++) {
    const PoseletSample& p_sample = sampels[i_sample];

    std::vector<PoseletSample::Leaf*> leafs;
    forest.evaluate(&p_sample, leafs);
    cv::Point_<int> center(p_sample.get_roi().x + patch_width/2,
                           p_sample.get_roi().y + patch_height/2);

    for(int i=0; i < leafs.size();i++) {
      float foreground = leafs[i]->forground;
      if(foreground == 0 ) {
        continue;
      }
      for(int j=0; j < leafs[i]->part_offsets.size(); j++) {
        cv::Point_<int> vote;
        int part_id = leafs[i]->part_indices[j];

        if(regression) { // part offsets stored in the leafs
          const Point_<int>& offset = leafs[i]->part_offsets[j];
          vote = center + offset;
        }else{ // part offsets stored in the poselet
          const Point_<int>& offset = poselets.part_offsets[j];
          vote = center + offset;

        }

        if (vote.x > 0 && vote.y > 0 && vote.x < roi.width && vote.y < roi.height ) {
          maps[part_id].at<float> (vote) += foreground/leafs.size();
        }
      }
    }
//    LOG(INFO) << p_sample.get_descriptor();
//    Mat plot = maps[0].clone();
//    cv::circle(plot, center, 3, cv::Scalar(255, 255, 255, 0), -1);
//    imshow("x", plot);
//    waitKey(0);

  }
//  if(step_size == 2) {
//    for(int i=0; i < maps.size(); i++) {
//      cv::blur(maps[i], maps[i], cv::Size(3,3));
//    }
//  }

}



void eval_voting(learning::forest::Forest<PoseletSample>& forest,
    Poselet& poselets,
    const learning::common::Image& sample,
    vector<Mat_<float> >* voting_maps,
    int step_size = 2,
    bool regression = true) {

  if(forest.trees.size() == 0)
    return;
  vector<Mat_<float> >& maps = *voting_maps;
  Rect roi(0,0,sample.width(),sample.height());
  learning::forest::ForestParam param = forest.getParam();
  int patch_width = param.patch_width;
  int patch_height = param.patch_height;
  for (int x = roi.x; x < roi.x + roi.width - patch_width; x += step_size) {
    for (int y = roi.y; y < roi.y + roi.height - patch_height; y += step_size) {

      cv::Rect patch_box(x, y, patch_width, patch_height);
      PoseletSample s(&sample, patch_box);

//      s.show(10);
//      cv::imshow("voting", maps[0]);
//      waitKey(0);

      std::vector<PoseletSample::Leaf*> leafs;
      forest.evaluate(&s, leafs);

      cv::Point_<int> center(x+patch_width/2, y+patch_height/2);

      for(int i=0; i < leafs.size();i++) {
        float foreground = leafs[i]->forground;
        if(foreground == 0 ) {
          continue;
        }
//        if(foreground > 0 ) {
//          int part_id = leafs[i]->part_indices[0];
//          maps[part_id].at<float>(center) += foreground/leafs.size();
//        }
//        continue;

        CHECK_EQ(poselets.part_offsets.size(), leafs[i]->part_offsets.size());

        for(int j=0; j < leafs[i]->part_offsets.size(); j++) {
          cv::Point_<int> vote;
          int part_id = leafs[i]->part_indices[j];

          if(regression) { // part offsets stored in the leafs
            const Point_<int>& offset = leafs[i]->part_offsets[j];
            vote = center + offset;
          }else{ // part offsets stored in the poselet
            const Point_<int>& offset = poselets.part_offsets[j];
            vote = center + offset;
          }

          if (vote.x > 0 && vote.y > 0 && vote.x < roi.width && vote.y < roi.height ) {
            maps[part_id].at<float> (vote) += foreground/leafs.size();
          }
        }
      }
    }
  }

  if(step_size == 2) {
    for(int i=0; i < maps.size(); i++) {
      cv::blur(maps[i], maps[i], cv::Size(3,3));
    }
  }

}


void eval_voting_poselets_mt(vector<learning::forest::Forest<PoseletSample> >& forests,
    vector<Poselet>& poselets,
    const learning::common::Image& sample,
    vector<Mat_<float> >* voting_maps,
    int step_size,
    bool regression) {

  vector<Mat_<float> >& maps = *voting_maps;
  maps.resize(13);
  Rect roi(0,0,sample.width(),sample.height());
  for(int i=0; i < maps.size(); i++){
    maps[i] = cv::Mat::zeros(roi.height, roi.width, cv::DataType<float>::type);
  }

  int num_threads = ::utils::system::get_available_logical_cpus();
  if(num_threads <= 1 ) {

    eval_voting_poselets( &forests, &poselets, &sample, voting_maps, step_size, regression );
  }else{

    vector<vector<Mat_<float> > > results(forests.size());
    vector<vector< learning::forest::Forest<PoseletSample> > > forests_splitted(forests.size());
    vector<vector<Poselet> > poselets_splitted(forests.size());

    boost::thread_pool::executor e(num_threads);
    for(unsigned int i = 0; i < forests_splitted.size(); i++) {
      forests_splitted[i].push_back(forests[i]);
      poselets_splitted[i].push_back(poselets[i]);
      e.submit(boost::bind( &eval_voting_poselets, &forests_splitted[i], &poselets_splitted[i], &sample, &results[i], step_size, regression));

    }
    e.join_all();

    for(int i_thread=0; i_thread < forests_splitted.size(); i_thread++) {
      for(int i_forest=0; i_forest < forests_splitted[i_forest].size(); i_forest++) {
         const Poselet& poselet = poselets_splitted[i_thread][i_forest];

         for(int i=0; i < poselet.part_indices.size(); i++) {
           int part_id = poselet.part_indices[i];
           cv::add(maps[part_id], results[i_thread][part_id], maps[part_id]);
         }
      }
    }
  }
}
void eval_voting_poselets( vector<learning::forest::Forest<PoseletSample> >* forests,
    vector<Poselet>* poselets,
    const learning::common::Image* sample,
    vector<Mat_<float> >* voting_maps,
    int step_size,
    bool use_regression) {

  vector<Mat_<float> >& maps = *voting_maps;
  Rect roi(0,0,sample->width(),sample->height());

  if(maps.size() == 0 ){
    maps.resize(13);
    Rect roi(0,0,sample->width(),sample->height());
    for(int i=0; i < maps.size(); i++){
      maps[i] = cv::Mat::zeros(roi.height, roi.width, cv::DataType<float>::type);
    }
  }

  for(int i=0; i < forests->size(); i++) {
//      eval_voting_hog( (*forests)[i], (*poselets)[i], *sample, voting_maps);
      eval_voting( (*forests)[i], (*poselets)[i], *sample, voting_maps, step_size, use_regression);
  }
}

} /* namespace poselets */
} /* namespace body_pose */
