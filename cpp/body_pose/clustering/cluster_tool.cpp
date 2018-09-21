/*
 * create_clusters.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: mdantone
 */


#include <opencv2/opencv.hpp>

#include <istream>
#include <boost/filesystem/path.hpp>
#include <boost/assign/std/vector.hpp>
#include "cpp/utils/file_utils.hpp"
#include "cpp/body_pose/utils.hpp"
#include "body_clustering.hpp"


using namespace boost::assign;
using namespace std;
using namespace cv;
using namespace body_pose::clustering;


int main(int argc, char** argv) {
  srand(1);
  vector<int> parents;
  get_joint_constalation(parents);

  vector<Annotation> annotations_;
  string index_file =  "/home/mdantone/scratch/data/leed_sport/index_train_flipped.txt";
  index_file = "/srv/glusterfs/mdantone/data/lookbook/index_rescaled_train_clean.txt";
  index_file = "/home/mdantone/scratch/data/leed_sport/index_train_both.txt";
  index_file = "/srv/glusterfs/mdantone/data/lookbook/index_rescaled_test_allparts.txt";
  load_annotations(annotations_, index_file);

  LOG(INFO) << annotations_.size() << " found.";
  vector<Annotation> annotations;
  clean_annotations(annotations_, annotations);
  CHECK_GT( annotations.size(), 0);
  LOG(INFO) << annotations.size() << " cleaned.";



  string path = "/scratch_net/giggo/mdantone/grid/poselets_fashwell/clusters/";

  ClusterMethod method = PART_POSE;
  method = GLOBAL_POSE;
//  method = GIST_APPEARANCE;
//  method = BOW_SPM;

  vector<int> cluster_sizes;
  cluster_sizes += 2,3,4,5,6,7,8,9,10,11,12,13,14,15,20;
  for(int j = 0; j < cluster_sizes.size(); j++) {
    int n_clusters = cluster_sizes[j];

    vector<Mat> clusters;
    // try to load clusters
    if(load_clusters(path, n_clusters, method, clusters) ) {
      CHECK_EQ(clusters.size(), parents.size());

      std::vector<int> part_indices;
      bool visualize = true;
      if(visualize) {
        if(method == PART_POSE) {
          for(int i=0; i < parents.size(); i++) {
            part_indices.push_back(i);
            part_indices.push_back( parents[i] );

            visualize_part_clusters(annotations, method, part_indices, clusters[i]);
          }
        }else{
          visualize_part_clusters(annotations, method, part_indices, clusters[0]);
        }
      }

      bool save = false;
      if(save){
        if(method != PART_POSE) {

          vector<Annotation> clustered_annotations;
          assigne_to_clusters (annotations, part_indices, clusters[0],
              method, clustered_annotations);
          CHECK( clustered_annotations.size() > 0 );
          save_clustrered_annotations(path, index_file, n_clusters, method,
              clustered_annotations);
        }
        // store annotations
      }

      bool print = true;
      if(print){

      }
    }else{
      // create clusters and store them
      std::vector<int> part_indices;
      if(method == PART_POSE) {
        for(int i=0; i < parents.size(); i++) {
          part_indices.push_back(i);
          part_indices.push_back( parents[i] );
          Mat cluster;
          cluster_annotations(annotations, method, n_clusters, cluster, part_indices);
          clusters.push_back(cluster);
        }
      }else{
        Mat cluster;
        cluster_annotations(annotations, method, n_clusters, cluster, part_indices);

        for(int i=0; i < parents.size(); i++) {
          clusters.push_back(cluster);
        }
      }
      save_clusters(path, n_clusters, method, clusters);
    }
  }
  return 0;
}

