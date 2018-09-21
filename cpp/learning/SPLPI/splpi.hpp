/**
 * splpi.hpp
 *
 *  Created on: July 22, 2016
 *      Author: Umar Iqbal
 *  Re-Implementation of "Joint Subset Partition and Labeling
 *  for Multi Person Pose Estimation", CVPR-2016.
 **/


#ifndef SPLPI_HH
#define SPLPI_HH

#include "cpp/learning/SPLPI/detection.hpp"
#include "cpp/learning/logistic_regression/LogisticRegression.hpp"
#include "cpp/body_pose/body_pose_types.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <glog/logging.h>
#include "gurobi_c++.h"

#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "cpp/utils/libsvm/libsvm.hpp"

namespace learning
{

namespace splpi
{

typedef ::utils::libsvm::SVMHolder SVM;
typedef ::utils::libsvm::ProblemHolder SvmProblem;
typedef ::utils::libsvm::SolverParameter SvmParam;

struct PairwiseFeatHist{

  PairwiseFeatHist(){}
  PairwiseFeatHist(cv::Mat bins_, cv::Mat bin_freqs_, int n_bins_):
                n_bins(n_bins_){
    bins = bins_.clone();
    bin_freqs = bin_freqs_.clone();
  }
  cv::Mat bins;
  cv::Mat bin_freqs;
  int n_bins;
};

struct PairwiseDisplacement{
  PairwiseDisplacement():means(),covars(), weights(), cluster_size(0){}
  PairwiseDisplacement(std::vector<cv::Mat> means_, std::vector<cv::Mat> covars_, std::vector<float> weights_, int cluster_size_):
                      means(means_), covars(covars_), weights(weights_), cluster_size(cluster_size_){}
  std::vector<cv::Mat> means;
  std::vector<cv::Mat> covars;
  std::vector<float> weights;
  float cluster_size;
  cv::EM em;
};

struct BinaryModel{
  cv::Mat mean;
  cv::Mat std;
  std::string path;
  boost::shared_ptr<splpi::SVM> model;

  BinaryModel(): mean(), std(), path() {
    model.reset(new splpi::SVM);
  }

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & mean;
    ar & std;
    ar & path;
  }
};

class SPLPI
{
  public:

    // constructors
    SPLPI(unsigned int num_labels_,
          float norm_size_,
          body_pose::BodyPoseTypes pose_type_);

    SPLPI(unsigned int num_labels_,
          float norm_size_, body_pose::BodyPoseTypes pose_type_,
          bool use_single_solution_constraint_);

    // setters
    void set_num_labels(unsigned int num_labels_);
    bool set_objective_function(std::vector<Detection>& detections);
    void set_single_solution_contraint();

    bool set_unary_models(std::vector<BinaryModel>& models);
    bool set_same_class_binary_models(std::vector<BinaryModel>& models);
    bool set_diff_class_binary_models(std::vector<std::vector<splpi::BinaryModel> >& models);
    bool set_pairwise_displacements(std::vector<std::vector<cv::EM> >& displacements);

    // savers and loaders
    bool save_same_class_binary_models(std::string save_path);
    bool save_diff_class_binary_models(std::string save_path);
    bool save_unary_models(std::string save_path);
    bool load_same_class_binary_models(std::string path);
    bool load_diff_class_binary_models(std::string path);
    bool load_unary_models(std::string path);

    // getters
    cv::EM get_pairwise_displacment(int pIdx, int ppIdx);

    bool reset_model();
    bool initialize_model();

    int optimize(std::vector<Detection>& detections,
                 std::vector<std::vector<cv::Point> >& minimas);
    int optimize(std::vector<Detection>& detections,
                 std::vector<std::vector<cv::Point> >& minimas,
                 vector<vector<int> > &mst_parents);

    float refine_pose(const std::vector<cv::Mat_<float> >& appearance_scores,
                      const std::vector<cv::Point_<int> >& pose,
                      std::vector<cv::Point_<int> >& refined_pose,
                      body_pose::BodyPoseTypes pose_type,
                      std::vector<int> parents,
                      int radius = 10);

    float evaluate_pose(std::vector<Detection>& detections, vector<int> parents);

    // destructor
    virtual ~SPLPI();

  protected:

  private:

    // setter functions
    bool set_unaries(std::vector<Detection>& detections);

    double normalized_unary(int label, Detection d1);
    double same_class_binary(int label, Detection d1, Detection d2);
    double diff_class_binary(int label1, Detection d1, int label2, Detection d2);
    bool set_binaries(std::vector<Detection>& detections);

    // add constraints
    bool add_contraints();

    // finds connected components
    int connected_component(vector<long unsigned int>& component);

    // finds minimum spanning tree
    float minimum_spanning_tree(vector<size_t>& mst);


    float inference_dp(const std::vector<cv::Mat_<float> >& appearance_scores,
                  const std::vector<cv::Point_<int> >& pose,
                  std::vector< cv::Mat_<float> >& scores,
                  std::vector< cv::Mat_<int> >& score_src_x,
                  std::vector< cv::Mat_<int> >& score_src_y,
                  body_pose::BodyPoseTypes pose_type,
                  std::vector<int> parents,
                  int part_id,
                  int radius);

    // variables
    unsigned int num_labels;
    bool use_single_solution_constraint;
    float norm_size;
    body_pose::BodyPoseTypes pose_type;

    /// enviroment and model of gurobi
    GRBEnv* env;
    GRBModel* model;

    /// variable names as in the original paper
    std::vector<std::vector<GRBVar> > x;
    std::vector<std::vector<GRBVar> > y;
    std::vector<std::vector<std::vector<std::vector<GRBVar> > > > z;

    /// expressions to define objective function
    GRBLinExpr unaries;
    GRBLinExpr binaries;
    GRBLinExpr objective;

    /// models for binaries
    std::vector<std::vector<splpi::BinaryModel> > diff_class_binary_models;
    std::vector<splpi::BinaryModel> same_class_binary_models;
    std::vector<splpi::BinaryModel> unary_models;

    /// GMM for diff class binaries
    std::vector<std::vector<cv::EM> > pairwise_displacements;

    std::vector<int> peak_ids;

};
}
}
#endif // SPLP_HH
