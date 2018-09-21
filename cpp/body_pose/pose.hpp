#ifndef POSE_HPP
#define POSE_HPP

#include "cpp/utils/serialization/opencv_serialization.hpp"

namespace body_pose{

struct Pose{
  std::vector<cv::Point_<int> > parts;
  double inferenz_score; // total cost for the part
  double init_part_id; //id of the root part
  std::vector<double> part_wise_costs;

  // each row corresponds to probabilities of ith part.
  cv::Mat_<float> probs;

  Pose():parts(), inferenz_score(0), init_part_id(0), part_wise_costs(){}

  Pose(std::vector<cv::Point_<int> > parts_, double inferenz_score_):
                    parts(parts_), inferenz_score(inferenz_score_), init_part_id(0), part_wise_costs(){}

  Pose(std::vector<cv::Point_<int> > parts_, double inferenz_score_, double init_part_id_):
                    parts(parts_), inferenz_score(inferenz_score_), init_part_id(init_part_id_), part_wise_costs(){}

  Pose(std::vector<cv::Point_<int> > parts_, double inferenz_score_, double init_part_id_, std::vector<double> part_wise_costs_):
                    parts(parts_), inferenz_score(inferenz_score_), init_part_id(init_part_id_), part_wise_costs(part_wise_costs_){}

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & parts;
    ar & inferenz_score;
    ar & init_part_id;
    ar & part_wise_costs;
  }
};

}

#endif POSE_HPP
