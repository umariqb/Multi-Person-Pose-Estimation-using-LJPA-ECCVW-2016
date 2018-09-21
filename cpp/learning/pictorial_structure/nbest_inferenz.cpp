/*!
*
* Class implements pictorial structure inference to obtain
* n best poses from an image following the article:
*
* D. Park, D. Ramanan. "N-Best Maximal Decoders for Part Models"
* International Conference on Computer Vision (ICCV) Barcelona, Spain,
* November 2011.
*
* @Author: uiqbal
* @Date: 06.06.2014
*
*/

#include "cpp/learning/pictorial_structure/nbest_inferenz.hpp"
#include "cpp/learning/pictorial_structure/utils.hpp"
#include "cpp/learning/pictorial_structure/math_util.hpp"
#include "cpp/learning/pictorial_structure/learn_model_parameter.hpp"
#include "cpp/learning/forest/param.hpp"

#include "cpp/utils/thread_pool.hpp"
#include "cpp/utils/system_utils.hpp"
#include "cpp/utils/string_utils.hpp"

#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


using namespace std;
using namespace cv;
namespace learning
{
namespace ps
{
  NBestInferenz::~NBestInferenz()
  {
    //dtor
  }

  /*!
  * Function takes the cost image and computes minimum
  * values by doing non minimum/maximum supression.
  */
  bool NBestInferenz::nms_loop(const cv::Mat_<float>& cost_img,
                               std::vector<cv::Point_<int> >& min_locs,
                               float threshold, int patch_size, int max_count){
      cv::Mat_<float> cost = cost_img.clone();

      for(int i=0; i<max_count; i++){
        // get minimum
        Point_<int> min_index;
        double min_val = 0;
        minMaxLoc(cost, &min_val, 0, &min_index, 0);

        if(min_val > threshold){
          break;
        }
        min_locs.push_back(min_index);

        Rect box(min_index.x - patch_size/2, min_index.y - patch_size/2, patch_size, patch_size);
        Rect inter = utils::intersect(box, Rect(0,0,cost.cols,cost.rows));
        cost(inter).setTo(cv::Scalar(0));

//        cout<<min_val<<"\t"<<min_index.x<<"\t"<<min_index.y<<"\n";
//        imshow("cost", 1-cost);
//        waitKey(0);
      }
      return true;
  }


  /*!
  * Function changes the root in the graph and
  * shifts the parents accordingly.
  *
  * @param:
  *    part_id: is the part which we want to make root
  */
  bool NBestInferenz::reorder_parts(int new_root, std::vector<int>& new_parents, body_pose::BodyPoseTypes pose_type)
  {
    std::vector<int> parents;
    get_joint_constalation(parents, pose_type);
    get_joint_constalation(new_parents, pose_type);
    int par = parents[new_root];

    new_parents[par] = new_root;
    new_parents[new_root] = new_root;

    while(par != 0){
      int par_par = parents[par];
      new_parents[par_par] = par;
      par = par_par;
    }
    return true;
  }

  bool NBestInferenz::compute_arg_mins_mm(int part_id,
                                std::vector<cv::Point_<int> >& min_locs,
                                std::vector<int>& old_parents,
                                std::vector<int>& new_parents)
  {

    cv::Point_<int> par_loc = min_locs[part_id];

    // getting the childerns of part_id
    std::vector<int> child_ids;
    for(unsigned int i=0; i < new_parents.size(); i++){
      if(new_parents[i] == part_id && i != part_id){
        child_ids.push_back(i);
      }
    }

    // recursive call
    for(unsigned int i=0; i < child_ids.size(); i++){
      int cid = child_ids[i];
      cv::Point_<int> min_loc;
      if(old_parents[cid] == new_parents[cid]){
        min_loc.x = score_src_x[cid](par_loc);
        min_loc.y = score_src_y[cid](par_loc);
      }
      else{
        min_loc.x = score_src_x_d[part_id](par_loc);
        min_loc.y = score_src_y_d[part_id](par_loc);
      }
      min_locs[cid] = min_loc;
      compute_arg_mins_mm(child_ids[i], min_locs, old_parents, new_parents);
    }
    return true;
  }

  bool NBestInferenz::compute_diverse_poses(learning::ps::NBestParam param,
                                      body_pose::BodyPoseTypes pose_type)
  {
    CHECK(min_marginals_computed);
    unsigned int num_parts = min_marginals.size();
    std::vector<int> parents;
    get_joint_constalation(parents, pose_type);


    for(unsigned int pIdx=0; pIdx<param.nbest_part_ids.size(); pIdx++){

      unsigned int part_id = param.nbest_part_ids[pIdx];
      // extract several minimas
      std::vector<cv::Point> min_locs;
      nms_loop(min_marginals[part_id], min_locs, 0, param.nms_radius, param.max_poses);

      // make current part as root and reorder parents
      std::vector<int> new_parents;
      reorder_parts(part_id, new_parents, pose_type);
      CHECK_EQ(num_parts, new_parents.size());

      // compute argmins for each minima
      for(unsigned int i=0; i<min_locs.size(); i++){

        std::vector<cv::Point_<int> > parts(num_parts);
        parts[part_id] = min_locs[i];
        compute_arg_mins_mm(part_id, parts, parents, new_parents);

        std::vector<double> partwise_costs;
        if(param.save_partwise_costs){
          // saving contribution of each part to the cost function
          partwise_costs.resize(num_parts,0);

          // getting appearance and deformation cost for each part
          for(unsigned int j=0; j < parts.size(); j++){
            int parent_id = parents[j];
            double deformation_cost = scores[j](parts[parent_id]) -
                                  scores_without_dt[j](parts[j]);

//            LOG(INFO)<<scores[j].at<float>(parts[parent_id]) -
//                                  scores_without_dt[j].at<float>(parts[j]);

            //LOG(INFO)<<scores[j](parts[parent_id])<<"\t\t"<<scores_without_dt[j](parts[j]);
            //double appearance_cost = scores_without_dt[j](parts[j]);
            Part* part = model->get_part(part_id);

            const cv::Mat* app_score = part->get_appeareance_score();
            double appearance_cost = app_score->at<float>(parts[j]);

            // overall contribution of the part
            partwise_costs[j] = deformation_cost + appearance_cost;
            //cout<<partwise_costs[j]<<" ";
          }
          //cout<<endl;
        }
        Pose ps(parts, min_marginals[part_id](min_locs[i]), part_id, partwise_costs);
        poses.push_back(ps);

      }
    }
    return true;
  }

  bool NBestInferenz::compute_diverse_poses_for_peaks(learning::ps::NBestParam param, body_pose::BodyPoseTypes pose_type)
  {
    int max_count = 500;
    CHECK(min_marginals_computed);
    unsigned int num_parts = min_marginals.size();
    std::vector<int> parents;
    get_joint_constalation(parents, pose_type);

    /// generate peaks for given part ids
    vector<Point> peaks;
    vector<int> pids;
    for(unsigned int pIdx=0; pIdx<param.part_ids_for_peaks.size(); pIdx++){
      int part_id = param.part_ids_for_peaks[pIdx];
      Part* part = model->get_part(part_id);
      const cv::Mat_<float> app_score = *(const Mat_<float>*)part->get_appeareance_score();

      vector<Point> p_peaks;
      nms_loop(app_score, p_peaks, -1*param.thresh, 2*param.nms_radius_for_peaks);

      if(false){
        Mat s = app_score.clone();
        for(unsigned int p=0; p<p_peaks.size(); p++){
          circle(s, p_peaks[p], 3, Scalar(255,255,255), 2);
        }
        imshow("Peaks", s);
        waitKey(0);
      }

      peaks.insert(peaks.end(), p_peaks.begin(), p_peaks.end());
      pids.resize(pids.size()+p_peaks.size(), part_id);
    }

    int num_peaks = peaks.size();
    vector<vector<Pose> > poses_for_each_peak(num_peaks);

    // compute argmins for each peak
    for(unsigned int i=0; i<peaks.size(); i++){

      int part_id = pids[i];

      std::vector<int> new_parents;
      reorder_parts(part_id, new_parents, pose_type);
      CHECK_EQ(num_parts, new_parents.size());

      std::vector<cv::Point_<int> > parts(num_parts);
      parts[part_id] = peaks[i];
      compute_arg_mins_mm(part_id, parts, parents, new_parents);

      std::vector<double> partwise_costs;
      if(param.save_partwise_costs){

        Part* part = model->get_part(part_id);
        const cv::Mat* app_score = part->get_appeareance_score();

        // saving contribution of each part to the cost function
        partwise_costs.resize(num_parts,0);

        // getting appearance and deformation cost for each part
        for(unsigned int j=0; j < parts.size(); j++){
          int parent_id = parents[j];
          double deformation_cost = scores[j](parts[parent_id]) -
                                scores_without_dt[j](parts[j]);
          double appearance_cost = app_score->at<float>(parts[j]);
          // overall contribution of the part
          partwise_costs[j] = deformation_cost + appearance_cost;
        }
      }
      Pose ps(parts, min_marginals[part_id](peaks[i]), part_id, partwise_costs);
      poses_for_each_peak[i].push_back(ps);
    }


    vector<int> rem_nbest_parts_ids;
    set_difference(param.nbest_part_ids.begin(), param.nbest_part_ids.end(),
                     param.part_ids_for_peaks.begin(), param.part_ids_for_peaks.end(),
                     std::inserter(rem_nbest_parts_ids, rem_nbest_parts_ids.begin()));

    for(unsigned int pIdx=0; pIdx<rem_nbest_parts_ids.size(); pIdx++){

      unsigned int part_id = rem_nbest_parts_ids[pIdx];
      // extract several minimas
      std::vector<cv::Point> min_locs;
      nms_loop(min_marginals[part_id], min_locs, 0, 2*param.nms_radius, max_count);

      // make current part as root and reorder parents
      std::vector<int> new_parents;
      reorder_parts(part_id, new_parents, pose_type);
      CHECK_EQ(num_parts, new_parents.size());

      // compute argmins for each minima
      for(unsigned int i=0; i<min_locs.size(); i++){


        std::vector<cv::Point_<int> > parts(num_parts);
        parts[part_id] = min_locs[i];
        compute_arg_mins_mm(part_id, parts, parents, new_parents);

        bool pass_through_peak = false;
        int pid = 0;
        for(unsigned int p=0; p<peaks.size(); p++){
          int peak_id    = pids[p];
          Point peak_loc  = peaks[p];
          Point loc       = parts[peak_id];
          float dist     = sqrt(pow((loc.x - peak_loc.x),2)
                                   + pow((loc.y - peak_loc.y),2));
          if(dist <= param.nms_radius){
            pass_through_peak = true;
            pid = p; break;
          }
        }

        if(!pass_through_peak) continue;

        std::vector<double> partwise_costs;
        if(param.save_partwise_costs){
          // saving contribution of each part to the cost function
          partwise_costs.resize(num_parts,0);

          // getting appearance and deformation cost for each part
          for(unsigned int j=0; j < parts.size(); j++){
            int parent_id = parents[j];
            double deformation_cost = scores[j](parts[parent_id]) -
                                  scores_without_dt[j](parts[j]);

            Part* part = model->get_part(part_id);
            const cv::Mat* app_score = part->get_appeareance_score();
            double appearance_cost = app_score->at<float>(parts[j]);
            partwise_costs[j] = deformation_cost + appearance_cost;             // overall contribution of the part
          }
        }
        Pose ps(parts, min_marginals[part_id](min_locs[i]), part_id, partwise_costs);
        poses_for_each_peak[pid].push_back(ps);
      }
    }
    for(unsigned int i=0; i<poses_for_each_peak.size(); i++){
      std::sort(poses_for_each_peak[i].begin(),
                 poses_for_each_peak[i].end(),
                 NBestInferenz::by_inferenz_score());

      int N = std::min(static_cast<unsigned int>(poses_for_each_peak[i].size()), param.num_nbest);
      poses_for_each_peak[i].resize(N);
      poses.insert(poses.end(), poses_for_each_peak[i].begin(), poses_for_each_peak[i].end());

//      vector<Pose> cleaned_poses;
//      eliminate_overlapping_poses(poses_for_each_peak[i], cleaned_poses, 0.1);
//      N = std::min(static_cast<int>(cleaned_poses.size()), num_nbest);
//      poses.insert(poses.end(), cleaned_poses.begin(), cleaned_poses.end());
    }
    return true;
  }

  bool NBestInferenz::normalize_min_marginals(){

    unsigned int num_parts = min_marginals.size();
    for(unsigned int i=0; i<num_parts; i++){
      scores[i] /= static_cast<float>(num_parts);
      scores_without_dt[i] /= static_cast<float>(num_parts);
      min_marginals[i] /= static_cast<float>(num_parts);
    }
  }

  bool NBestInferenz::compute_min_marginals(){

    CHECK(min_computed);
    CHECK(scores.size());
    CHECK(score_src_x_d.size());
    CHECK(score_src_y_d.size());

    unsigned int nParts = scores.size();
    cv::Mat_<float> score_tmp;
    cv::Mat_<float> score_tmp_dt;

    //min marginals for root are already available
    min_marginals[0] = scores[0];

    // computing min_marginals for all other parts
    for(unsigned part_id=1; part_id < nParts; ++part_id){
      const Part* part = model->get_part(part_id);
      int parent_id = model->get_parent_id(part_id);
      score_tmp = min_marginals[parent_id] - scores[part_id];
      part->transform_d(score_tmp,
          score_tmp_dt,
          score_src_x_d[part_id],
          score_src_y_d[part_id]);

      min_marginals[part_id] = score_tmp_dt + scores_without_dt[part_id];
    }

    min_marginals_computed = true;
    return min_marginals_computed;
  }

  bool NBestInferenz::get_poses(std::vector<Pose>& out_poses){
    out_poses.insert(out_poses.end(), poses.begin(), poses.end());
    return true;
  }

  bool NBestInferenz::get_poses(std::vector<std::vector<cv::Point_<int> > >& out_poses){

    int out_poses_size = out_poses.size();
    out_poses.resize(out_poses_size+poses.size());

    for(unsigned int i=0; i<poses.size(); i++){
      out_poses[i]=poses[i-out_poses_size].parts;
    }
    return true;
  }

  bool eliminate_overlapping_poses(std::vector<body_pose::Pose>& poses
                                   ,std::vector<body_pose::Pose>& nms_poses,
                                   double threshold, int upper_body_size)
  {
    CHECK(poses.size());
    nms_poses.clear();
    vector<bool> is_suppressed(poses.size(), false);

    for(unsigned int i=0; i < poses.size(); i++){
      if(is_suppressed[i]){
        continue;
      }
      for(unsigned int j=i+1; j < poses.size(); j++){
        if(is_suppressed[j]){
          continue;
        }

        body_pose::Pose& pose_a = poses[i];
        body_pose::Pose& pose_b = poses[j];

        // pose is overlapping if all parts overlap
        int ov_parts = 0;
        for(unsigned int n=0; n<pose_a.parts.size(); n++){
          double dist = sqrt(pow((pose_a.parts[n].x - pose_b.parts[n].x), 2) +
                          pow((pose_a.parts[n].y - pose_b.parts[n].y), 2));

          if(dist <= threshold*upper_body_size){
            ov_parts++;
          }
        }

        if(ov_parts == pose_a.parts.size()){
          if(pose_a.inferenz_score >= pose_b.inferenz_score){
            is_suppressed[i] = true;
            break;
          }
          else{
            is_suppressed[j] = true;
          }
        }
      }
    }

    for(unsigned int i=0; i<poses.size(); i++){
      if(!is_suppressed[i]){
        nms_poses.push_back(poses[i]);
      }
    }
    return true;
  }

  /*!***************************************************
  *       Function to get nbest maximum decoders
  ******************************************************/
  float inferenz_nbest_max_decoder(vector<Model> models,
      const vector<cv::Mat_<float> >& apperance_scores,
      vector<body_pose::Pose>& poses,
      body_pose::BodyPoseTypes pose_type,
      NBestParam param,
      bool debug,
      const cv::Mat& img)
  {

    CHECK(models.size());

    // seting voting maps;
    vector<NBestInferenz> solvers;
    for(int j=0; j < models.size(); j++) {
      models[j].set_voting_maps(apperance_scores, -1);
      solvers.push_back(NBestInferenz(&models[j]) );
    }

    // multithreaded inferenz
    int num_threads = ::utils::system::get_available_logical_cpus();
    if(num_threads > 1 && models.size() > 1 && !debug) {

      LOG(INFO)<<"MULTITHREADIUNG";
      // dynamic programming first pass
      boost::thread_pool::executor e(num_threads);
      for(unsigned int i=0; i < solvers.size(); i++) {
        e.submit(boost::bind(&NBestInferenz::compute_min, &solvers[i], 0 ));
      }
      e.join_all();

      // dynamic programming 2nd pass
      // computing min marginals for each part
      boost::thread_pool::executor f(num_threads);
      for(unsigned int i=0; i < solvers.size(); i++){
        f.submit(boost::bind(&NBestInferenz::compute_min_marginals, &solvers[i]));
      }
      f.join_all();

//      boost::thread_pool::executor g(num_threads);
//      for(unsigned int i=0; i < solvers.size(); i++){
//        g.submit(boost::bind(&NBestInferenz::normalize_min_marginals, &solvers[i]));
//      }
//      g.join_all();

      //computing nbest poses using each model
      boost::thread_pool::executor h(num_threads);
      for(unsigned int i=0; i < solvers.size(); i++){
        //h.submit(boost::bind(&NBestInferenz::compute_diverse_poses, &solvers[i], threshold, save_partwise_costs, pose_type));
      }
      h.join_all();

    // singlethreaded inferenz
    }else{
      for(unsigned int i=0; i < solvers.size(); i++) {
        // dynamic programming first pass
        solvers[i].compute_min();
        // dynamic programming 2nd pass
        // computing min marginals for each part
        solvers[i].compute_min_marginals();
        //normalize min_marginals between 0 and 1
        solvers[i].normalize_min_marginals();
        // computing nbest poses
        if(param.part_ids_for_peaks.size()){
          solvers[i].compute_diverse_poses_for_peaks(param, pose_type);
        }
        else{
          solvers[i].compute_diverse_poses(param, pose_type);
        }
      }
    }

    // obtain poses from all models
    for(unsigned int i=0; i<solvers.size(); i++){
      solvers[i].get_poses(poses);
    }

    return 0;
  }

  float inferenz_nbest_max_decoder(std::vector<Model> models,
      const vector<Mat_<float> >& apperance_scores,
      vector<vector<cv::Point_<int> > >& min_locations,
      body_pose::BodyPoseTypes pose_type,
      NBestParam param,
      bool debug,
      const cv::Mat& img)
  {

    std::vector<body_pose::Pose> poses;

    inferenz_nbest_max_decoder(models, apperance_scores, poses,pose_type, param, debug, img);

    int n_poses = std::min(param.num_nbest, static_cast<unsigned int>(poses.size()));
    min_locations.clear();
    min_locations.resize(n_poses);

    for(unsigned int i=0; i<n_poses; i++){
      min_locations[i]=poses[i].parts;

      if(debug){
        LOG(INFO)<<"Part_id = "<<poses[i].init_part_id<<"  Score = "<<poses[i].inferenz_score;
      }
    }
    return 0;
  }

  bool load_nbest_poses(std::string path, std::vector<body_pose::Pose>& poses)
  {
    std::ifstream ifs(path.c_str());

    if(!ifs){
      LOG(INFO)<<"file not found.";
    }
    else{
      try{
        boost::archive::text_iarchive ia(ifs);
        ia>>poses;
        LOG(INFO)<<"Poses loaded";
        return true;
      }
      catch(boost::archive::archive_exception& ex){
        LOG(INFO)<<"Reload Tree: Archive exception during deserializiation: "
              <<ex.what();
          LOG(INFO)<<"not able to load poses from: "<<path;
      }
    }
    return false;
  }

  bool save_nbest_poses(std::vector<body_pose::Pose>& poses, std::string path){
    try{
      std::ofstream ofs(path.c_str());
      if(ofs==0){
      LOG(INFO)<<"Error: Cannot open the given path to save detected poses.";
      return false;
      }
      boost::archive::text_oarchive oa(ofs);
      oa<<poses;
      ofs.flush();
      ofs.close();
      LOG(INFO)<<"Poses saved at :"<<path;
      return true;
    }
    catch(boost::archive::archive_exception& ex){
      LOG(INFO)<<"Archive exception during deserialization:" <<std::endl;
      LOG(INFO)<< ex.what() << std::endl;
      LOG(INFO)<< "it was file: "<<path;
    }
    return true;
  }

} /* namespace ps */
} /* namespace learning */

