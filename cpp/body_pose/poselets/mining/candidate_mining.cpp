#include <opencv2/opencv.hpp>

#include <istream>
#include <boost/filesystem/path.hpp>
#include <boost/assign/std/vector.hpp>
#include "cpp/utils/file_utils.hpp"
#include "cpp/body_pose/utils.hpp"
#include "cpp/body_pose/clustering/body_clustering.hpp"
#include "cpp/body_pose/poselets/poselets.hpp"
#include "cpp/body_pose/poselets/training/train_utils.hpp"

using namespace boost::assign;
using namespace std;
using namespace cv;
using namespace body_pose::clustering;
using namespace body_pose::poselets;

#include <gflags/gflags.h>

void get_double_poselets( vector< Poselet >& poselets) {

  Poselet sholder_head_left;
  sholder_head_left.name = "sholder_head_left";
  sholder_head_left.part_indices += 0,2;
  poselets.push_back( sholder_head_left );

  Poselet sholder_head_right;
  sholder_head_right.name = "sholder_head_right";
  sholder_head_right.part_indices += 0,1;
  poselets.push_back( sholder_head_right );

  Poselet lower_leg_left;
  lower_leg_left.name = "lower_leg_left";
  lower_leg_left.part_indices += 11,12;
  poselets.push_back( lower_leg_left );

  Poselet lower_leg_right;
  lower_leg_right.name = "lower_leg_right";
  lower_leg_right.part_indices += 9,10;
  poselets.push_back( lower_leg_right );

  Poselet upper_leg_left;
  upper_leg_left.name = "upper_leg_left";
  upper_leg_left.part_indices += 4,11;
  poselets.push_back( upper_leg_left );

  Poselet upper_leg_right;
  upper_leg_right.name = "upper_leg_right";
  upper_leg_right.part_indices += 3,9;
  poselets.push_back( upper_leg_right );

  Poselet lower_arm_left;
  lower_arm_left.name = "lower_arm_left";
  lower_arm_left.part_indices += 7,8;
  poselets.push_back( lower_arm_left );

  Poselet lower_arm_right;
  lower_arm_right.name = "lower_arm_right";
  lower_arm_right.part_indices += 5,6;
  poselets.push_back( lower_arm_right );

  Poselet upper_arm_left;
  upper_arm_left.name = "upper_arm_left";
  upper_arm_left.part_indices += 2,7;
  poselets.push_back( upper_arm_left );

  Poselet upper_arm_right;
  upper_arm_right.name = "upper_arm_right";
  upper_arm_right.part_indices += 1,5;
  poselets.push_back( upper_arm_right );

  Poselet sholder_hip_right;
  sholder_hip_right.name = "sholder_hip_right";
  sholder_hip_right.part_indices += 1,3;
  poselets.push_back( sholder_hip_right );

  Poselet sholder_hip_left;
  sholder_hip_left.name = "sholder_hip_left";
  sholder_hip_left.part_indices += 2,4;
  poselets.push_back( sholder_hip_left );

}

void get_single_poselets( vector< Poselet >& poselets) {
  std::vector<std::string> pair_names;
  pair_names += "head", "shoulder_r", "shoulder_l", "hip_r", "hip_l","elbow_r","hand_r","elbow_l","hand_l","knee_r","feet_r","knee_l","feet_l";
  for(int i=0; i < 13; i++) {
    Poselet p;
    p.name = pair_names[i];
    p.part_indices += i;
    poselets.push_back(p);
  }
}

void get_tripple_poselets( vector< Poselet >& poselets) {
  Poselet left_leg;
  left_leg.name = "left_leg";
  left_leg.part_indices += 4,11,12;
  poselets.push_back( left_leg );

  Poselet right_leg;
  right_leg.name = "right_leg";
  right_leg.part_indices += 3,9,10;
  poselets.push_back( right_leg );

  Poselet left_arm;
  left_arm.name = "left_arm";
  left_arm.part_indices += 2,7,8;
  poselets.push_back( left_arm );

  Poselet right_arm;
  right_arm.name = "right_arm";
  right_arm.part_indices += 1,5,6;
  poselets.push_back( right_arm );

  Poselet head_sholder;
  head_sholder.name = "head_sholder";
  head_sholder.part_indices += 0,1,2;
  poselets.push_back( head_sholder );

}

void get_quadruple_poselets( vector< Poselet >& poselets) {

  Poselet left_arm_hip;
  left_arm_hip.name = "left_arm_hip";
  left_arm_hip.part_indices += 2,4,7,8;
  poselets.push_back( left_arm_hip );

  Poselet right_arm_hip;
  right_arm_hip.name = "right_arm_hip";
  right_arm_hip.part_indices += 1,3,5,6;
  poselets.push_back( right_arm_hip );

  Poselet left_arm_head;
  left_arm_head.name = "left_arm_head";
  left_arm_head.part_indices += 2,0,7,8;
  poselets.push_back( left_arm_head );

  Poselet right_arm_head;
  right_arm_head.name = "right_arm_head";
  right_arm_head.part_indices += 1,0,5,6;
  poselets.push_back( right_arm_head );

  Poselet torso;
  torso.name = "torso";
  torso.part_indices += 1,2,3,4;
  poselets.push_back( torso );

  Poselet lower_legs;
  lower_legs.name = "lower_legs";
  lower_legs.part_indices += 9,10,11,12;
  poselets.push_back( lower_legs );

  Poselet upper_legs;
  upper_legs.name = "upper_legs";
  upper_legs.part_indices += 3,4,9,11;
  poselets.push_back( upper_legs );


  Poselet upper_arm;
  upper_arm.name = "upper_arm";
  upper_arm.part_indices += 1,2,5,7;
  poselets.push_back( upper_arm );

}

void get_poselets( vector< Poselet >& poselets ) {
  // full body
//  Poselet full_body;
//  full_body.name = "full_body";
//  full_body.part_indices += 0,1,2,3,4,5,6,7,8,9,10,11,12;

  Poselet upper_body;
  upper_body.name = "upper_body";
  upper_body.part_indices += 1,2,3,4,5,6,7,8;

  Poselet lower_body;
  lower_body.name = "lower_body";
  lower_body.part_indices += 9,10,11,12;

  Poselet lower_body_2;
  lower_body_2.name = "lower_body_2";
  lower_body_2.part_indices += 3,4,9,10,11,12;

  Poselet left_arm_torso;
  left_arm_torso.name = "left_arm_torso";
  left_arm_torso.part_indices += 1,2,3,4,7,8;

  Poselet rigth_arm_torso;
  rigth_arm_torso.name = "rigth_arm_torso";
  rigth_arm_torso.part_indices += 1,2,3,4,5,6;

  Poselet left_arm;
  left_arm.name = "left_arm";
  left_arm.part_indices += 2,7,8;

  Poselet left_lower_arm;
  left_lower_arm.name = "left_lower_arm";
  left_lower_arm.part_indices += 7,8;

  Poselet right_arm;
  right_arm.name = "right_arm";
  right_arm.part_indices += 5,6;

  Poselet right_lower_arm;
  right_lower_arm.name = "right_lower_arm";
  right_lower_arm.part_indices += 1,5,6;

  Poselet left_leg;
  left_leg.name = "left_leg";
  left_leg.part_indices += 4,11,12;

  Poselet left_lower_leg;
  left_lower_leg.name = "left_lower_leg";
  left_lower_leg.part_indices += 11,12;

  Poselet right_leg;
  right_leg.name = "right_leg";
  right_leg.part_indices += 3,9,10;

  Poselet right_lower_leg;
  right_lower_leg.name = "right_lower_leg";
  right_lower_leg.part_indices += 9,10;

  Poselet head_left;
  head_left.name = "head_left";
  head_left.part_indices += 0,2;

  Poselet head_right;
  head_right.name = "head_right";
  head_right.part_indices += 0,1;


//  poselets.push_back(full_body);
  poselets.push_back(upper_body);
  poselets.push_back(lower_body);
  poselets.push_back(lower_body_2);
  poselets.push_back(left_arm_torso);
  poselets.push_back(rigth_arm_torso);
  poselets.push_back(left_arm);
  poselets.push_back(left_lower_arm);
  poselets.push_back(right_arm);
  poselets.push_back(right_lower_arm);
  poselets.push_back(left_leg);
  poselets.push_back(left_lower_leg);
  poselets.push_back(right_leg);
  poselets.push_back(right_lower_leg);
  poselets.push_back(head_left);
  poselets.push_back(head_right);
}
void get_parents(std::vector<int>& parents) {
  parents += 0,0,0,1,2,1,5,2,7,3,9,4,11;
}

DEFINE_int32(num_clusters, 0, "i_tree");
DEFINE_bool(save_cluster, false, "save clusters");
DEFINE_string(save_path, "", "save clusters");

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  const int num_clusters = FLAGS_num_clusters;
  const bool save_cluster = FLAGS_save_cluster;

  srand(1);
  vector<int> parents;
  get_joint_constalation(parents);

  string save_path = "/home/mdantone/scratch/grid/poselets_fashwell/poselets/" + FLAGS_save_path;
  LOG(INFO) << save_path;

  // load annotations
  vector<Annotation> annotations_;
  string index_file = "/home/mdantone/scratch/data/leed_sport/index_train_both.txt";
//index_file = "/srv/glusterfs/mdantone/data/lookbook/index_rescaled_train_both.txt";
  index_file = "/scratch_net/giggo/mdantone/grid/poselets_fashwell/train_set.txt";
  load_annotations(annotations_, index_file);


  // clean all annotations, we only use the images where all parts are present
  LOG(INFO) << annotations_.size() << " found.";
  vector<Annotation> annotations;
  clean_annotations(annotations_, annotations);
  CHECK_GT( annotations.size(), 0);
  LOG(INFO) << annotations.size() << " cleaned.";



//  vector<Poselet > poselets = get_poselets();
  vector<Poselet > poselets;
  get_single_poselets(poselets);
//  get_double_poselets(poselets);
//  get_tripple_poselets(poselets);
//  get_quadruple_poselets(poselets);

  for( int i =0; i < poselets.size(); i++) {

    ClusterMethod method = MULTI_PARTS_POSE;
    vector<Annotation> clustered_annotations;

    int n_clusters = num_clusters;
    vector<int> part_indices = poselets[i].part_indices;
    if(poselets[i].part_indices.size() == 1 ) {
      method = PART_POSE;
      vector<int> parents;
      get_parents(parents);
      part_indices.push_back(parents[poselets[i].part_indices[0]] );
    }

    if(n_clusters > 1) {
      Mat cluster;
      cluster_annotations(annotations, method, n_clusters, cluster, part_indices);
      assigne_to_clusters (annotations, part_indices, cluster,
          method, clustered_annotations);

      if(!save_cluster) {
        visualize_part_clusters(annotations, method, part_indices, cluster);
      }

    }else{
      clustered_annotations = annotations;
      for(int ii=0; ii < clustered_annotations.size(); ii++) {
        clustered_annotations[ii].cluster_id = 0;
      }
    }

    for(int j=0; j < n_clusters; j++) {
      Poselet p = poselets[i];
      for(int ii=0; ii < clustered_annotations.size(); ii++) {
        if( clustered_annotations[ii].cluster_id == j) {
          p.annotations.push_back( clustered_annotations[ii] );
        }else{
          p.neg_annotations.push_back( clustered_annotations[ii] );
        }
      }

      LOG(INFO) << "cluster "  << j << ", pos: " << p.annotations.size() << " , neg: " << p.neg_annotations.size();
      if(p.annotations.size() < 25 ) {
        LOG(INFO) << "not enought "<< p.annotations.size();
        continue;
      }


      // calculate bbox
//      int default_size = 48;
      int default_size = 38;

      body_pose::poselets::calculate_poselet_bbox( p.annotations, p.part_indices, 1.1, default_size);

      p.poselet_size = p.annotations[0].bbox;
      p.poselet_size.x = 0;
      p.poselet_size.y = 0;


      // calculate distribution of offset-vectors
      if(true) {
        p.part_offsets.resize( p.part_indices.size(), Point_<int>(0,0));
        for(int ii=0; ii <  p.annotations.size(); ii++) {
          Annotation ann =  p.annotations[ii];
          Point center_poselet( ann.bbox.x + ann.bbox.width / 2,
                                ann.bbox.y + ann.bbox.height / 2);

          for(int i_index=0; i_index <  p.part_indices.size(); i_index++) {
            int part_index = p.part_indices[i_index];
            p.part_offsets[i_index].x += (ann.parts[part_index].x - center_poselet.x);
            p.part_offsets[i_index].y += (ann.parts[part_index].y - center_poselet.y);
          }
        }
        if(p.annotations.size() > 0 ) {
          for(int i_index=0; i_index <  p.part_indices.size(); i_index++) {
            float x = static_cast<float>(p.part_offsets[i_index].x) / p.annotations.size();
            float y = static_cast<float>(p.part_offsets[i_index].y) / p.annotations.size();


            p.part_offsets[i_index].x = static_cast<int>(x);
            p.part_offsets[i_index].y = static_cast<int>(y);
            LOG(INFO) << "AVG: "<< i_index << " (" << p.part_offsets[i_index].x << ", " << p.part_offsets[i_index].y << ")";

          }
        }
      }

      if(!save_cluster) {
        for(int ii=0; ii < 2; ii++) {
          Mat img = imread( p.annotations[ii].url );
          LOG(INFO) << p.annotations[ii].url ;
          rectangle(img, p.annotations[ii].bbox, cv::Scalar(255, 255, 255, 0));

          Point center_poselet(  p.annotations[ii].bbox.x +  p.annotations[ii].bbox.width / 2,
                                 p.annotations[ii].bbox.y +  p.annotations[ii].bbox.height / 2);

          for(int i_index=0; i_index <  p.part_indices.size(); i_index++) {
            Point offset_part = p.part_offsets[i_index] + center_poselet;
            cv::circle(img, offset_part, 3, cv::Scalar(255, 255, 255, 0), -1);
          }

          LOG(INFO) <<  p.annotations[ii].cluster_id;
          imshow("example", img);
          waitKey(0);
        }
      }


      string f_name(boost::str(boost::format("%1%/%2%_%3%.poselet" ) % save_path % i % j ));

      if( save_cluster) {
        CHECK(save_poselet(p, f_name));
      }
      LOG(INFO) << i << " " << j << " -> " << p.annotations.size() << " " << f_name ;

    }
  }
}
