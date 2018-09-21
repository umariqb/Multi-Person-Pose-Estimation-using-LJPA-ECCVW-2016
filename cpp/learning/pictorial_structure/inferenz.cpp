/*!
 * inferenz.cpp
 *
 *  Created on: Mar 24, 2013
 *  @Author: mdantone
 *
 *  @Modified by: uiqbal
 *  Date:06.06.2014
 */
#include "cpp/learning/pictorial_structure/inferenz.hpp"
#include "cpp/learning/pictorial_structure/utils.hpp"
#include "cpp/learning/pictorial_structure/math_util.hpp"
#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/vision/geometry_utils.hpp"

#include "cpp/utils/thread_pool.hpp"
#include "cpp/utils/system_utils.hpp"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
namespace learning
{
namespace ps
{

  float Inferenz::compute_min(int part_id) {
    CHECK_GT(scores.size(), part_id);

    const Part* part = model->get_part(part_id);
    vector<int> child_ids = model->get_children_ids(part_id);
    Mat_<float> score_tmp = part->get_appeareance_score_copy();

    // these scores are already be shifted
    for( int i = 0; i < child_ids.size(); i++){
      int child_id = child_ids[i];
      compute_min(child_id);
      add(score_tmp, scores[child_id], score_tmp);
    }

    // saving scores without dt to compute the min_marginals later on
    scores_without_dt[part_id] = score_tmp.clone();

    bool is_root = !model->has_parent(part_id);
    if(!is_root) {
      part->transform(score_tmp,
          scores[part_id],
          score_src_x[part_id],
          score_src_y[part_id]);
    }else{
      scores[part_id] = score_tmp.clone();
    }

    // check if root
    if( is_root ) {
      min_computed = true;
      double min;
      minMaxLoc(scores[0], &min, 0 , 0, 0);
      _inferenz_score =  static_cast<float>(min);
      return static_cast<float>(min);
    }else{
      return 0;
    }
  }

  void Inferenz::compute_arg_min( vector<Point_<int> >& min_locations,
                                  int part_id) const {
    CHECK_NOTNULL(scores[part_id].data);
    CHECK_EQ(min_locations.size(), scores.size());

    // the anker point is the location of the parent
    // and for the root the anker point is the maximum of the score mat
    Point_<int> min_loc;
    if(model->has_parent(part_id)) {
      Point anker;
      CHECK_GE(model->get_parent_id(part_id), 0);
      anker = min_locations[ model->get_parent_id(part_id) ];
      CHECK_NE(anker.x, -1);
      min_loc.x = score_src_x[part_id](anker);
      min_loc.y = score_src_y[part_id](anker);
    }else{
      minMaxLoc(scores[part_id], 0, 0, &min_loc, 0);
      CHECK_NE(min_loc.x, -1);
    }

    min_locations[part_id] = min_loc;

    // recursive call
    vector<int> child_ids = model->get_children_ids(part_id);
    for(int i=0; i < child_ids.size(); i++) {
      compute_arg_min( min_locations, child_ids[i]);
    }
  }

  void Inferenz::non_max_supression( vector<vector<Point_<int> > >& locations,
      int n_locations) {

    Mat score = scores[0].clone();
    normalize(score, score, 0, 1, CV_MINMAX);
    vector<int> child_ids = model->get_children_ids(0);
    int n_parts = model->get_num_parts();
    for(int i=0; i < n_locations; i++) {
      vector<Point_<int> >l(n_parts, cv::Point(-1,-1));

      // get maximum
      Point_<int> anker;
      minMaxLoc(score, 0, 0, &anker, 0);

      l[0].x = score_src_x[0](anker);
      l[0].y = score_src_y[0](anker);

      //TODO: The following for loop does not seems to be
      // neccesary since compute_arg_min has recursive calls
      for(int i=0; i < child_ids.size(); i++) {
        compute_arg_min( l, child_ids[i]);
      }
      locations.push_back(l);

      int patch_size = 20;
      Rect box(anker.x - patch_size/2, anker.y - patch_size/2, patch_size, patch_size);
      Rect inter = utils::intersect(box, Rect(0,0,score.cols,score.rows));
      score(inter).setTo(cv::Scalar(0));
    }
  }

  float inferenz_multiple(vector<Model> models,
      const vector<cv::Mat_<float> >& apperance_scores,
      vector<cv::Point_<int> >& min_locations, bool debug,
      const Mat& img, float weight, int num_threads) {

    // seting voting maps;
    vector<Inferenz> solvers;
    for(int j=0; j < models.size(); j++) {
      models[j].set_voting_maps(apperance_scores, -1);
      solvers.push_back(Inferenz(&models[j]) );
    }

    // multithreaded inferenz
    if(num_threads < 1){
        num_threads = ::utils::system::get_available_logical_cpus();
    }
    if(num_threads > 1 && models.size() > 1 && !debug) {
      boost::thread_pool::executor e(num_threads);
      for(int i=0; i < solvers.size(); i++) {
        e.submit(boost::bind(&Inferenz::compute_min, &solvers[i], 0 ));
      }
      e.join_all();
    // singlethreaded inferenz
    }else{
      for(int i=0; i < solvers.size(); i++) {
        solvers[i].compute_min();
      }
    }

    // check min
    float min_inferenz = boost::numeric::bounds<double>::highest();
    int min_index = 0;
    for(int i=0; i < solvers.size(); i++) {
      // logarithmic weighting
      float w = -log(models[i].get_weight()) * weight;
      float inferenz = solvers[i].get_score() + w;
      if(debug){
        LOG(INFO)<<"weight = "<<w<<"\t\tinferenz = "<<solvers[i].get_score()<<"\t\ttotal "<<inferenz;
      }

      if( inferenz < min_inferenz ) {
        min_inferenz = inferenz;
        min_index = i;
      }
    }
    if(debug and solvers.size() > 1) {
      for(int i=0; i < solvers.size(); i++) {
        solvers[i].compute_arg_min(min_locations);
        Mat p = img.clone();
      }
    }
    solvers[min_index].compute_arg_min(min_locations);

    return min_inferenz;
  }


  void add_parents_to_graph(vector<int>& parts, vector<int>& parents){

    vector<int> parts_to_add;
    for(size_t i=0; i<parts.size(); i++){
      int part_id   = parts[i];
      int parent_id = parents[parts[i]];

      // handle root case e.g two detection 0 & 9
      if(parent_id == part_id){
        parts_to_add.push_back(parent_id);
      }
      else if(!(std::find(parts.begin(), parts.end(), parent_id) != parts.end())){
        if(!(std::find(parts_to_add.begin(), parts_to_add.end(), parent_id) != parts_to_add.end())){
          parts_to_add.push_back(parent_id);
        }
      }
    }
//    LOG(INFO)<<::utils::VectorToString(parts_to_add);
    if(parts_to_add.size() > 1){
      add_parents_to_graph(parts_to_add, parents);
    }
    parts.insert(parts.end(), parts_to_add.begin(), parts_to_add.end());
    return;
  }

  float inferenz_multiple_refinement(vector<Model> models,
      const vector<Mat_<float> >& apperance_scores,
      const vector<Point_<int> >& min_locations,
      vector<Point_<int> >& refined_locations,
      vector<float>& mean_lengths,
      body_pose::BodyPoseTypes pose_type,
      std::vector<int> parents,
      bool debug,
      Mat img, float weight, int num_threads) {


    CHECK_EQ(min_locations.size(), apperance_scores.size());
    CHECK_EQ(refined_locations.size(), apperance_scores.size());
    vector<int> visible_parts, recon_parts;

    if(!parents.size()){
      get_joint_constalation(parents, pose_type);
    }

    vector<Model> new_models(models.size());
    vector<vector<Part> > parts(models.size());
    for(size_t i=0; i<min_locations.size(); i++){
      if(min_locations[i].x >= 0 && min_locations[i].y >= 0){
        visible_parts.push_back(i);
      }
      else{
        parents[i] = -1;
      }

      int part_id = i;
      int parent_id = parents[i];

      if(parent_id == -1){
        parent_id = 0;
      }

      int i_part = parent_id*(pose_type)+part_id;
      //int i_part = part_id;
//      LOG(INFO)<<i_part;

      for(size_t m=0; m<new_models.size(); m++){
        parts[m].push_back(*models[m].get_part(i_part));
        Part* part = models[m].get_part(i_part);
//        LOG(INFO)<<part->get_name();
      }
    }

    if(visible_parts.size()<=1)
      return 0;

    for(size_t m=0; m<new_models.size(); m++){
      new_models[m].set_parts(parts[m], parents);
    }

    vector<float> scales;
    int root = 0;
    for(size_t i=0; i<visible_parts.size(); i++){
      int part_id = visible_parts[i];
      int parent_id = parents[part_id];

      CHECK_NE(parent_id, -1);

      int i_part = parent_id*(pose_type)+part_id;

      if(part_id == parent_id){
        root = part_id;
        continue;
      }

      Point pt1 = min_locations[part_id];
      Point pt2 = min_locations[parent_id];

      if(!is_valid(pt1) || !is_valid(pt2)){
        continue;
      }

      float length = cv::norm(pt1-pt2);

      float scale = mean_lengths[i_part]/length;
      scales.push_back(scale);
    }
//    LOG(INFO)<<::utils::VectorToString(scales);
    std::sort(scales.begin(), scales.end());
//    LOG(INFO)<<::utils::VectorToString(scales);

    float scale = 1;
    if(scales.size()) {scale = scales[scales.size()/2];}

    vector<Mat_<float> > new_app_scores(apperance_scores.size());
    for(size_t i=0; i<apperance_scores.size(); i++){
      resize(apperance_scores[i], new_app_scores[i], Size(), scale, scale);
      GaussianBlur(new_app_scores[i], new_app_scores[i], Size(), 5, 5);
    }

    int radius = 15;
//    Mat tmp_img = img;
//    resize(img, tmp_img, Size(), scale, scale);
    for(size_t i=0; i<visible_parts.size(); i++){
      int part_id = visible_parts[i];
      Point loc = min_locations[part_id];
      loc.x *= scale;
      loc.y *= scale;
      Mat mask = Mat::zeros(new_app_scores[i].size(), CV_32FC1);
      circle(mask, loc, radius, Scalar(1,1,1), -1);
      multiply(new_app_scores[part_id], mask, new_app_scores[part_id]);


//      vector<Mat> planes(3);
//      split(tmp_img,planes);
//      Mat tmp_mask = new_app_scores[part_id]*255;
//      tmp_mask.convertTo(tmp_mask, CV_8U);
//      planes[2] += tmp_mask;
//      cv::merge(planes, tmp_img);
//
////      new_app_scores[part_id] += mask*10;
////      new_app_scores[part_id] += mask*10;
//      imshow("masked_image", tmp_img);
//      imshow("test", new_app_scores[part_id]);
//      waitKey(0);
    }

    // seting voting maps;
    vector<Inferenz> solvers;
    for(int m=0; m < models.size(); m++) {
      new_models[m].set_parents(parents);
      new_models[m].set_voting_maps(new_app_scores, -1);
      solvers.push_back(Inferenz(&new_models[m]) );
    }
    // multithreaded inferenz
    if(num_threads < 1){
        num_threads = ::utils::system::get_available_logical_cpus();
    }

    for(int i=0; i < solvers.size(); i++) {
      solvers[i].compute_min(root);
    }

    // check min
    float min_inferenz = boost::numeric::bounds<double>::highest();
    int min_index = 0;
    for(int i=0; i < solvers.size(); i++) {
      // logarithmic weighting
      float w = -log(models[i].get_weight()) * weight;
      float inferenz = solvers[i].get_score() + w;

      if(debug){
        LOG(INFO)<<"weight = "<<w<<"\t\tinferenz = "<<solvers[i].get_score()<<"\t\ttotal "<<inferenz;
      }

      if( inferenz < min_inferenz ) {
        min_inferenz = inferenz;
        min_index = i;
      }
    }

    if(debug and solvers.size() > 1) {
      for(int i=0; i < solvers.size(); i++) {
        solvers[i].compute_arg_min(refined_locations, root);
        Mat p = img.clone();
      }
    }
    solvers[min_index].compute_arg_min(refined_locations, root);

//    _mplot_14(tmp_img, refined_locations, Scalar(0,255,0), "", 0, false, "overlay");

    for(size_t i=0; i<refined_locations.size(); i++){
      if(is_valid(refined_locations[i])){
        refined_locations[i].x /= scale;
        refined_locations[i].y /= scale;
      }
    }
//    LOG(INFO)<<min_inferenz;
//    resize(tmp_img, img, Size(), 1.0/scale, 1.0/scale);
//    imshow("testtt", img);
//    waitKey(0);
    return min_inferenz;
  }

  // also tells which PS model was selected by taking min_index
  // as reference (applicable only when mixture of PS model is used)
  float inferenz_multiple(vector<Model> models,
      const vector<cv::Mat_<float> >& apperance_scores,
      vector<cv::Point_<int> >& min_locations, int& min_index,
       bool debug, const Mat& img, float weight, int num_threads) {

    // seting voting maps;
    vector<Inferenz> solvers;
    for(int j=0; j < models.size(); j++) {
      models[j].set_voting_maps(apperance_scores, -1);
      solvers.push_back(Inferenz(&models[j]) );
    }

    // multithreaded inferenz
    if(num_threads < 1){
        num_threads = ::utils::system::get_available_logical_cpus();
    }

    if(num_threads > 1 && models.size() > 1 && !debug) {
      boost::thread_pool::executor e(num_threads);
      for(int i=0; i < solvers.size(); i++) {
        e.submit(boost::bind(&Inferenz::compute_min, &solvers[i], 0 ));
      }
      e.join_all();

    // singlethreaded inferenz
    }else{
      for(int i=0; i < solvers.size(); i++) {
        solvers[i].compute_min();
      }
    }

    // check min
    float min_inferenz = boost::numeric::bounds<double>::highest();
    min_index = 0;
    for(int i=0; i < solvers.size(); i++) {
      // logarithmic weighting
      float w = -log(models[i].get_weight()) * weight;
      float inferenz = solvers[i].get_score() + w;
      if(debug){
        LOG(INFO)<<"weight = "<<w<<"\t\tinferenz = "<<solvers[i].get_score()<<"\t\ttotal "<<inferenz;
      }

      if( inferenz < min_inferenz ) {
        min_inferenz = inferenz;
        min_index = i;
      }
    }
    //LOG(INFO)<<"min_index = "<<min_index;

    if(debug and solvers.size() > 1) {
      for(int i=0; i < solvers.size(); i++) {
        solvers[i].compute_arg_min(min_locations);
        Mat p = img.clone();
        //plot( p, min_locations);
      }
    }
    solvers[min_index].compute_arg_min(min_locations);

    return min_inferenz;
  }

  float inferenz_multiple_remove_duplicates(vector<Model> models,
      const vector<Mat_<float> >& apperance_scores,
      const vector<vector<Point_<int> > >& min_locations,
      vector<vector<Point_<int> > >& refined_locations,
      vector<float>& mean_lengths,
      body_pose::BodyPoseTypes pose_type,
      float thresh,
      bool debug,
      const Mat& img, float weight, int num_threads) {


   // CHECK_EQ(min_locations.size(), apperance_scores.size());
   // CHECK_EQ(refined_locations.size(), apperance_scores.size());

    vector<vector<vector<Point_<int> > > > dup_locs;
    vector<bool> suppressed(min_locations.size(), false);
    for(size_t i=0; i<min_locations.size(); i++){

      if(suppressed[i]){ continue; }

      dup_locs.resize(dup_locs.size()+1);
      suppressed[i] = true;
      const vector<Point_<int> > &parts1 = min_locations[i];
      dup_locs.back().push_back(parts1);

      for(size_t j=i+1; j<min_locations.size(); j++){

        if(suppressed[j]){ continue; }

        const vector<Point_<int> > &parts2 = min_locations[j];

        int n_duplicates = 0;
        int vis_count1 = 0, vis_count2 = 0;
        for(size_t p=0; p<parts1.size(); p++){
          Point_<int> pt1 = parts1[p];
          Point_<int> pt2 = parts2[p];

          if(pt1.x >= 0  &&pt1.y >= 0){ vis_count1++; }
          if(pt2.x >= 0  &&pt2.y >= 0){ vis_count2++; }

          if(pt1.x < 0  || pt1.y < 0 || pt2.x < 0 || pt2.y < 0){
            continue;
          }

          float dist = sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2));
          if(dist <= thresh){
            n_duplicates++;
          }
        }

        int min_parts = min(vis_count1, vis_count2);
        if(n_duplicates >= std::round(min_parts/2.0)){
          suppressed[j] = true;
          dup_locs.back().push_back(parts2);
        }
      }
    }

    vector<int> parents;
    get_joint_constalation(parents, pose_type);

    refined_locations.resize(dup_locs.size());
    for(size_t d=0; d<dup_locs.size(); d++){

      if(dup_locs[d].size() == 1){
        refined_locations[d] = dup_locs[d][0];
        continue;
      }

      // calculate scale
      vector<float> scales;
      for(size_t i=0; i<dup_locs[d].size(); i++){
        vector<Point_<int> > &parts = dup_locs[d][i];
        for(size_t part_id=0; part_id<parts.size(); part_id++){

          int parent_id = parents[part_id];

          if(part_id == parent_id){
            continue;
          }

          Point pt1 = min_locations[i][part_id];
          Point pt2 = min_locations[i][parent_id];

          if(!is_valid(pt1) || !is_valid(pt2)){
            continue;
          }

          float length = cv::norm(pt1-pt2);
          float scale = mean_lengths[part_id]/length;
          scales.push_back(scale);
        }
      }

      std::sort(scales.begin(), scales.end());
      float scale = 1;
      if(scales.size())
        scale = scales[scales.size()/2];


      refined_locations[d].resize((int)pose_type, Point(-1, -1));
      vector<Mat_<float> > new_app_scores(apperance_scores.size());
      vector<Mat_<float> > scaled_app_scores(apperance_scores.size());
      for(size_t p=0; p<apperance_scores.size(); p++){
        resize(apperance_scores[p], scaled_app_scores[p], Size(), scale, scale);
        new_app_scores[p] = Mat::zeros(scaled_app_scores[p].size(), CV_32FC1);
      }

      vector<int> visible_parts;
      for(size_t i=0; i<dup_locs[d].size(); i++){
        vector<Point_<int> > &parts = dup_locs[d][i];

        for(size_t p=0; p<parts.size(); p++){
          Point loc = parts[p];
          loc.x *= scale;
          loc.y *= scale;

          if(loc.x < 0 && loc.y < 0){ continue; }

          visible_parts.push_back(p);
          Mat mask = Mat::zeros(new_app_scores[i].size(), CV_32FC1);
          circle(mask, loc, 15, Scalar(2,2,2), -1);
          GaussianBlur(mask, mask, Size(), 5, 5);
          multiply(mask, scaled_app_scores[p], mask);
          new_app_scores[p] = new_app_scores[p] + mask;
//          imshow("test",  new_app_scores[p]);
//          waitKey(0);

//          multiply(new_app_scores[p], mask, new_app_scores[p]);
        }
      }

      sort(visible_parts.begin(), visible_parts.end());
      vector<int>::iterator it;
      it = unique(visible_parts.begin(), visible_parts.end());
      visible_parts.resize(distance(visible_parts.begin(),it));

      // define new joint constalation
      vector<int> new_parents(parents.size(), -1);
      int root = visible_parts[0];
      new_parents[root] = root;
      for(size_t i=1; i<visible_parts.size(); i++){
        int part_id = visible_parts[i];
        new_parents[part_id] = parents[part_id];
      }

      // seting voting maps;
      vector<Inferenz> solvers;
      for(int j=0; j < models.size(); j++) {
        models[j].set_parents(new_parents);
        models[j].set_voting_maps(new_app_scores, -1);
        solvers.push_back(Inferenz(&models[j]) );
      }
      // multithreaded inferenz
      if(num_threads < 1){
          num_threads = ::utils::system::get_available_logical_cpus();
      }
      if(num_threads > 1 && models.size() > 1 && !debug) {
        boost::thread_pool::executor e(num_threads);
        for(int i=0; i < solvers.size(); i++) {
          e.submit(boost::bind(&Inferenz::compute_min, &solvers[i], 0 ));
        }
        e.join_all();
      // singlethreaded inferenz
      }else{
        for(int i=0; i < solvers.size(); i++) {
          solvers[i].compute_min(root);
        }
      }

      // check min
      float min_inferenz = boost::numeric::bounds<double>::highest();
      int min_index = 0;
      for(int i=0; i < solvers.size(); i++) {
        // logarithmic weighting
        float w = -log(models[i].get_weight()) * weight;
        float inferenz = solvers[i].get_score() + w;

        if( inferenz < min_inferenz ) {
          min_inferenz = inferenz;
          min_index = i;
        }
      }

      if(debug and solvers.size() > 1) {
        for(int i=0; i < solvers.size(); i++) {
          solvers[i].compute_arg_min(refined_locations[d], root);
          Mat p = img.clone();
        }
      }
      solvers[min_index].compute_arg_min(refined_locations[d], root);


      for(size_t i=0; i<refined_locations[d].size(); i++){
        if(is_valid(refined_locations[d][i])){
          refined_locations[d][i].x /= scale;
          refined_locations[d][i].y /= scale;
        }
      }
      LOG(INFO)<<min_inferenz;
    }
  }

  void inferenz_multiple_mt(std::vector<Model>* models,
      const std::vector<cv::Mat_<float> >* apperance_scores,
      std::vector<cv::Point_<int> >* min_locations, bool debug, const Mat* img, float weight, int num_threads, float* score){
      *score = learning::ps::inferenz_multiple(*models, *apperance_scores, *min_locations, debug, *img, weight, num_threads);
  }

  float inferenz_multiple_diff_votmap(std::vector<std::vector<Model> > models,
      const std::vector< std::vector<cv::Mat_<float> > >& apperance_scores,
      std::vector<int> indexes,
      std::vector<cv::Point_<int> >& min_locations, bool debug, const Mat& img, float weight, int num_threads) {

    std::vector<std::vector<cv::Point_<int> > > min_locations_tmp(apperance_scores.size());
    std::vector<float> scores(apperance_scores.size());
    float min_score = std::numeric_limits<float>::max();

    if(num_threads < 1){
        num_threads = ::utils::system::get_available_logical_cpus();
    }

    num_threads /= indexes.size();

    if(num_threads > 1 && apperance_scores.size() > 1 && !debug) {
      boost::thread_pool::executor e(num_threads);
      for(int i=0; i < apperance_scores.size(); i++) {
        min_locations_tmp[i].resize(apperance_scores[i].size(), Point(-1,-1));
        e.submit(boost::bind(&inferenz_multiple_mt, &models[indexes[i]], &apperance_scores[i], &min_locations_tmp[i], debug, &img, weight, num_threads, &scores[i]));
      }
      e.join_all();

      for(int i=0; i<apperance_scores.size(); i++){
        if(scores[i] < min_score){
          min_score = scores[i];
          min_locations = min_locations_tmp[i];
        }
      }

    // singlethreaded inferenz
    }else{
      for(int i=0; i < apperance_scores.size(); i++) {
          min_locations_tmp[i].resize(apperance_scores[i].size(), Point(-1,-1));
          float score = inferenz_multiple(models[indexes[i]], apperance_scores[i], min_locations_tmp[i], debug, img, weight, num_threads);
          if(score < min_score){
            min_score = score;
            min_locations = min_locations_tmp[i];
          }
      }
    }
    return min_score;
  }

} /* namespace ps */
} /* namespace learning */
