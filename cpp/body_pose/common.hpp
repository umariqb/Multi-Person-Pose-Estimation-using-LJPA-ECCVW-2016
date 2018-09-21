/*
 * common.hpp
 *
 *  Created on: Aug 26, 2013
 *      Author: mdantone
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <opencv2/opencv.hpp>
#include "cpp/utils/serialization/opencv_serialization.hpp"
#include <fstream>
#include <libconfig.h++>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include "cpp/body_pose/body_pose_types.hpp"
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;
using namespace libconfig;

static const int NUM_ENTRIES = 26 + 4 + 1 + 1 + 1; //parts_coords +  bbox + cluster_id + num_parts + img_url

struct Annotation {
    Annotation() : flipped(false),
                   cluster_id(0),
                   center(),
                   weight(1),
                   scale(1)
                   {};

    // number of parts
    std::vector<cv::Point> parts;

    // url to original image
    std::string url;

    // bounding box
    cv::Rect bbox;

    bool flipped;

    // cluster_id
    int cluster_id;

    cv::Point_<int> center;

    // position
    cv::Point_<int> position;

    // weight of the annotation
    double weight;

    // best scale for the annotation.
    double scale;

    std::vector<bool> is_visible;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & parts;
      ar & url;
      ar & bbox;
      ar & flipped;
      ar & cluster_id;
    }
};

void inline _get_joint_constalation_J17(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(2);
    parents.push_back(2);
    parents.push_back(2);
    parents.push_back(5);
    parents.push_back(6);
    parents.push_back(6);
    parents.push_back(3);
    parents.push_back(9);
    parents.push_back(4);
    parents.push_back(11);
    parents.push_back(7);
    parents.push_back(13);
    parents.push_back(8);
    parents.push_back(15);
}

void inline _get_joint_constalation_J15(std::vector<int>& parents){
//    parents.push_back(0);
//    parents.push_back(0);
//    parents.push_back(1);
//    parents.push_back(1);
//    parents.push_back(14);
//    parents.push_back(14);
//    parents.push_back(2);
//    parents.push_back(6);
//    parents.push_back(3);
//    parents.push_back(8);
//    parents.push_back(4);
//    parents.push_back(10);
//    parents.push_back(5);
//    parents.push_back(12);
//    parents.push_back(1);

    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(1);
    parents.push_back(2);
    parents.push_back(3);
    parents.push_back(2);
    parents.push_back(6);
    parents.push_back(3);
    parents.push_back(8);
    parents.push_back(4);
    parents.push_back(10);
    parents.push_back(5);
    parents.push_back(12);
    parents.push_back(1);

}

void inline _get_joint_constalation_J14(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(1);
    parents.push_back(2);
    parents.push_back(3);
    parents.push_back(2);
    parents.push_back(6);
    parents.push_back(3);
    parents.push_back(8);
    parents.push_back(4);
    parents.push_back(10);
    parents.push_back(5);
    parents.push_back(12);
}

void inline _get_joint_constalation_J13(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(2);
    parents.push_back(1);
    parents.push_back(5);
    parents.push_back(2);
    parents.push_back(7);
    parents.push_back(3);
    parents.push_back(9);
    parents.push_back(4);
    parents.push_back(11);
}

void inline _get_joint_constalation_J10(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(1);
    parents.push_back(1);
    parents.push_back(4);
    parents.push_back(2);
    parents.push_back(6);
    parents.push_back(3);
    parents.push_back(8);
}


void inline _get_joint_constalation_J9(std::vector<int>& parents){

    // Normall Case
//    parents.push_back(0);
//    parents.push_back(0);
//    parents.push_back(0);
//    parents.push_back(1);
//    parents.push_back(2);
//    parents.push_back(1);
//    parents.push_back(5);
//    parents.push_back(2);
//    parents.push_back(7);


    // FixMe: Add a separate flag for TUM Kitchen Datasets
    // For MPII Cooking Dataset
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(3);
    parents.push_back(4);
    parents.push_back(2);
    parents.push_back(6);
    parents.push_back(7);

}

void inline _get_joint_constalation_J7(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(3);
    parents.push_back(2);
    parents.push_back(5);
}

void inline _get_joint_constalation_J5(std::vector<int>& parents){
    parents.push_back(0);
    parents.push_back(0);
    parents.push_back(1);
    parents.push_back(2);
    parents.push_back(3);
}


void inline _get_joint_constalation_FC_J14(std::vector<int>& parents){
    parents.resize(14*14);
    for(unsigned int i=0; i<14; i++){
      for(unsigned int j=0; j<14; j++){
        parents[i*14+j] = i;
      }
    }
}


void inline _get_joint_constalation_temporal(std::vector<int>& parents,
                          int nNeighbours) {

    static const int basic_parents[] = {0, 0, 0, 1, 2, 1, 5, 2, 7, 3, 9, 4, 11};

    int img_count = nNeighbours*2+1;
    for(int idx = 0; idx < img_count*13; idx++){

      int parent_id = 0;
      if(idx < 13){
        parent_id = basic_parents[idx];
      }
      else{
        parent_id = (idx%13);
      }

      parents.push_back(parent_id);

    }
}


void inline get_joint_constalation(std::vector<int>& parents,
                            body_pose::BodyPoseTypes pose_type,
                            int nNeigbours = 0) {

    switch(pose_type){
      case body_pose::FULL_BODY_J15:{
        _get_joint_constalation_J15(parents);
        break;
      }
      case body_pose::FULL_BODY_J14:{
        _get_joint_constalation_J14(parents);
        break;
      }
      case body_pose::FULL_BODY_J13:{
        _get_joint_constalation_J13(parents);
        break;
      }
      case body_pose::FULL_BODY_J17:{
        _get_joint_constalation_J17(parents);
        break;
      }
      case body_pose::FULL_BODY_J13_TEMPORAL:{
        _get_joint_constalation_temporal(parents, nNeigbours);
      }
      case body_pose::UPPER_BODY_J7:{
        _get_joint_constalation_J7(parents);
        break;
      }
      case body_pose::UPPER_BODY_J9:{
        _get_joint_constalation_J9(parents);
        break;
      }
      case body_pose::UPPER_BODY_J10:{
        _get_joint_constalation_J10(parents);
        break;
      }
      case body_pose::FC_FULL_BODY_J14:{
        _get_joint_constalation_FC_J14(parents);
      }

    }
}


struct MultiAnnotation{
        MultiAnnotation() : persons(),
                 single_person(),
                 count(0),
                 url(),
                 id(0)
                 {};

  std::vector<Annotation> persons;
  std::vector<int> single_person;
  std::vector<int> person_groups;
  int count;
  std::string url;
  int id;
};

struct SequenceInfo{
  SequenceInfo():path(), frame_count(0) {};
  SequenceInfo(std::string _path, int _frame_count):path(_path), frame_count(_frame_count){};

  std::string path;
  int frame_count;
};

struct Join{
  Join(int a, int b, cv::Scalar color_ ) :part_a(a), part_b(b), color(color_){};
  int part_a;
  int part_b;
  cv::Scalar color;
};

void inline _plot(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0, bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(0,2,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,4,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(1,3,cv::Scalar(0,255, 255)) );

    joints.push_back( Join(1,5,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(5,6,cv::Scalar(255, 255, 0)) );

    joints.push_back( Join(2,7,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(7,8,cv::Scalar(255, 0, 255)) );

    joints.push_back( Join(4,11,cv::Scalar(255, 0, 0)) );
    joints.push_back( Join(11,12,cv::Scalar(255, 0, 0)) );

    joints.push_back( Join(3,9,cv::Scalar(0, 0, 255)) );
    joints.push_back( Join(9,10,cv::Scalar(0, 0, 255)) );


    for(unsigned int i=0; i < ann.size()-1; i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 3);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 13){
          cv::circle(plot, cv::Point_<int>(x, y), 5, cv::Scalar(0, 0, 0, 0), -1 );
        }
        else if(i < 26){
          cv::circle(plot, cv::Point_<int>(x, y), 3, cv::Scalar(255, 0, 0, 0), -1 );
        }
        else{
          cv::circle(plot, cv::Point_<int>(x, y), 3, cv::Scalar(0, 0, 255, 0), -1 );
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_7(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0,  bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(0,2,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,3,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(3,4,cv::Scalar(128,255, 128)) );

    joints.push_back( Join(2,5,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(5,6,cv::Scalar(255, 0, 255)) );

    for(unsigned int i=0; i < ann.size()-1; i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 1);
      }
    }

    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      cv::circle(plot, cv::Point_<int>(x, y), 0.5, cv::Scalar(0, 0, 0, 0), -1 );
 //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_9(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0,  bool overwrite = false ){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(0,2,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,3,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(3,4,cv::Scalar(128,255, 128)) );
    joints.push_back( Join(4,5,cv::Scalar(128,255, 128)) );

    joints.push_back( Join(2,6,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(6,7,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(7,8,cv::Scalar(255, 0, 255)) );

    for(unsigned int i=0; i < ann.size()-1; i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 1);
      }
    }

    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      cv::circle(plot, cv::Point_<int>(x, y), 0.5, cv::Scalar(0, 0, 0, 0), -1 );
 //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_14(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0,  bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,3,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,1,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(3,5,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(2,4,cv::Scalar(0,255, 255)) );

    joints.push_back( Join(2,6,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(6,7,cv::Scalar(255, 255, 0)) );

    joints.push_back( Join(3,8,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(8,9,cv::Scalar(255, 0, 255)) );

    joints.push_back( Join(5,12,cv::Scalar(255, 0, 0)) );
    joints.push_back( Join(12,13,cv::Scalar(255, 0, 0)) );

    joints.push_back( Join(4,10,cv::Scalar(0, 0, 255)) );
    joints.push_back( Join(10,11,cv::Scalar(0, 0, 255)) );


    for(unsigned int i=0; i < joints.size(); i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 2);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 14){
          cv::circle(plot, cv::Point_<int>(x, y), 3, cv::Scalar(0, 0, 0, 0), -1 );
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_10(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0,  bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,3,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,1,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,4,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(4,5,cv::Scalar(0,255, 255)) );

    joints.push_back( Join(2,6,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(6,7,cv::Scalar(255, 255, 0)) );

    joints.push_back( Join(3,8,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(8,9,cv::Scalar(255, 0, 255)) );


    for(unsigned int i=0; i < joints.size(); i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 3);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 10){
          cv::circle(plot, cv::Point_<int>(x, y), 5, cv::Scalar(0, 0, 0, 0), -1 );
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_5(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0,  bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(1,0,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(1,3,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,1,cv::Scalar(0, 255, 0)) );

    joints.push_back( Join(3,5,cv::Scalar(0,255, 255)) );
    joints.push_back( Join(2,4,cv::Scalar(0,255, 255)) );

    joints.push_back( Join(2,6,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(6,7,cv::Scalar(255, 255, 0)) );

    joints.push_back( Join(3,8,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(8,9,cv::Scalar(255, 0, 255)) );


    for(unsigned int i=0; i < joints.size(); i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 3);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 10){
          cv::circle(plot, cv::Point_<int>(x, y), 5, cv::Scalar(0, 0, 0, 0), -1 );
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}


void inline _mplot_14(cv::Mat& img, std::vector<cv::Point> ann, cv::Scalar line_color,
              std::string path = "",
              int wait = 0,
              bool overwrite = false,
              std::string name = "Pose",
              std::vector<int> parents = std::vector<int>()){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    if(!parents.size()){
      get_joint_constalation(parents, body_pose::FULL_BODY_J14);
    }
    std::vector<Join> joints;

    for(size_t i=1; i<14; i++){
      joints.push_back(Join(i, parents[i], line_color));
    }

    std::vector<cv::Scalar> joint_colors;
    joint_colors.push_back(cv::Scalar(0,255,0)); // head
    joint_colors.push_back(cv::Scalar(0,0,255)); // neck
    joint_colors.push_back(cv::Scalar(255, 255, 0)); // l_sh
    joint_colors.push_back(cv::Scalar(255, 0, 0)); // r_sh
    joint_colors.push_back(cv::Scalar(255, 255, 0)); // l_hip
    joint_colors.push_back(cv::Scalar(255, 0, 0)); // r_hip
    joint_colors.push_back(cv::Scalar(255,0,255)); // l_elb
    joint_colors.push_back(cv::Scalar(0,255,255)); // l_hand
    joint_colors.push_back(cv::Scalar(0,0,255)); // r_elb
    joint_colors.push_back(cv::Scalar(0,255,0)); // r_hand
    joint_colors.push_back(cv::Scalar(255,0,255)); // l_knee
    joint_colors.push_back(cv::Scalar(0,255,255)); // l_ank
    joint_colors.push_back(cv::Scalar(0,0,255)); // r_knee
    joint_colors.push_back(cv::Scalar(0,255,0)); // r_ank

    for(unsigned int i=0; i < joints.size(); i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 2);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 14){
          cv::circle(plot, cv::Point_<int>(x, y), 4, joint_colors[i], -1 );
          cv::circle(plot, cv::Point_<int>(x, y), 5, line_color, 2);
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow(name, plot);
      cv::waitKey(wait);
    }else{
      cv::imwrite(path,plot);
    }
}

void inline _plot_17(cv::Mat& img, std::vector<cv::Point> ann, std::string path = "", int wait = 0, bool overwrite = false){
    cv::Mat plot;

    if( path != "" || overwrite) {
      plot = img;
    }else{
      plot = img.clone();
    }

    std::vector<Join> joints;

    joints.push_back( Join(0,1,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,1,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,3,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(2,4,cv::Scalar(0, 255, 0)) );

    joints.push_back( Join(2,5,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(5,6,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(6,7,cv::Scalar(0, 255, 0)) );
    joints.push_back( Join(6,8,cv::Scalar(0, 255, 0)) );

    joints.push_back( Join(3,9,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(9,10,cv::Scalar(255, 255, 0)) );
    joints.push_back( Join(4,11,cv::Scalar(255, 0, 255)) );
    joints.push_back( Join(11,12,cv::Scalar(255, 0, 255)) );

    joints.push_back( Join(8,15,cv::Scalar(0, 0, 255)) );
    joints.push_back( Join(15,16,cv::Scalar(0, 0, 255)) );

    joints.push_back( Join(7,13,cv::Scalar(255, 0, 0)) );
    joints.push_back( Join(13,14,cv::Scalar(255, 0, 0)) );


    for(unsigned int i=0; i < joints.size(); i++) {
      cv::Point a = ann[joints[i].part_a];
      cv::Point b = ann[joints[i].part_b];
      if(a.x >0 && b.x > 0 && a.y >0 && b.y > 0 && a.x < plot.cols && b.x < plot.cols ) {
        cv::line(plot, a, b, joints[i].color, 3);
      }
    }
    for (int i = 0; i < static_cast<int> (ann.size()); i++) {
      int x = ann[i].x;
      int y = ann[i].y;
      if (x > 0 and x < plot.cols and y > 0 and y < plot.rows) {
        if(i < 17){
          cv::circle(plot, cv::Point_<int>(x, y), 5, cv::Scalar(0, 0, 0, 0), -1 );
        }
        else if(i < 34){
          cv::circle(plot, cv::Point_<int>(x, y), 3, cv::Scalar(255, 0, 0, 0), -1 );
        }
        else{
          cv::circle(plot, cv::Point_<int>(x, y), 3, cv::Scalar(0, 0, 255, 0), -1 );
        }
      }
  //    rectangle(plot, ann.bbox, Scalar(0, 0, 255), 3);
    }

    if(path == "") {
      cv::imshow("Pose", plot);
      cv::waitKey(wait);
    }else{
      cv::imshow("Check", plot);
      //cv::imwrite(path,plot);
    }
}

// displays the annotations
void inline plot(cv::Mat& img, std::vector<cv::Point> ann,
                 body_pose::BodyPoseTypes pose_type, std::string path = "", int wait = 0, bool overwrite = false ){

      if(pose_type == body_pose::FULL_BODY_J13 ||
            pose_type ==  body_pose::FULL_BODY_J13_TEMPORAL){
                  _plot(img, ann, path, wait, overwrite);
      }
      else if(pose_type == body_pose::UPPER_BODY_J7){
        _plot_7(img, ann, path, wait, overwrite);
      }
      else if(pose_type == body_pose::FULL_BODY_J14 || pose_type == body_pose::FULL_BODY_J15){
        _plot_14(img, ann, path, wait, overwrite);
      }
      else if(pose_type == body_pose::FULL_BODY_J17){
        _plot_17(img, ann, path, wait, overwrite);
      }
      else if(pose_type == body_pose::UPPER_BODY_J9){
        _plot_9(img, ann, path, wait, overwrite);
      }
      else if(pose_type == body_pose::UPPER_BODY_J10){
        _plot_10(img, ann, path, wait, overwrite);
      }

}

// displays the annotations
void inline mplot(cv::Mat& img, std::vector<cv::Point> ann,
                 body_pose::BodyPoseTypes pose_type,
                 cv::Scalar line_color,
                 std::string path = "", int wait = 0,
                 bool overwrite = false ){

      if(pose_type == body_pose::FULL_BODY_J14 || pose_type == body_pose::FULL_BODY_J15){
        _mplot_14(img, ann, line_color, path, wait, overwrite);
      }
}


void inline plot(cv::Mat& img, MultiAnnotation ann,
                 body_pose::BodyPoseTypes pose_type,
                 std::string path = "", int wait = 0,
                bool overwrite = false )
{
  cv::Mat p;
  if( path != "" || overwrite) {
    p = img;
  }else{
    p = img.clone();
  }
  for(size_t i=0; i<ann.persons.size(); i++){
    mplot(p, ann.persons[i].parts, pose_type, cv::Scalar(0,0,255), path, 1, true);
  }
  cv::waitKey(wait);
}

int inline get_num_parts(body_pose::BodyPoseTypes pose_type)
{
    switch(pose_type){
      case body_pose::FULL_BODY_J15:
        return 15;
      case body_pose::FULL_BODY_J14:
        return 14;
      case body_pose::FULL_BODY_J13:
        return 13;
      case body_pose::FULL_BODY_J17:
        return 17;
      case body_pose::UPPER_BODY_J7:
        return 7;
      case body_pose::UPPER_BODY_J9:
        return 9;
      case body_pose::UPPER_BODY_J10:
        return 10;
      case body_pose::FC_FULL_BODY_J14:
        return 14;
      default:
        LOG(ERROR)<<"Undefined pose type provided.";
    }
}

void inline clean_annotations(const std::vector< Annotation>& annotations_org,
    std::vector< Annotation >& annotations) {
  for(unsigned int i=0; i < annotations_org.size(); i++) {
    bool all_parts_present = true;
    for(int j=0; j  < annotations_org[i].parts.size(); j++) {
      const cv::Point p = annotations_org[i].parts[j];
      if(p.x <= 0 || p.y <= 0 ) {
        all_parts_present = false;
      }
    }
    if(all_parts_present) {
      annotations.push_back(annotations_org[i]);
    }
  }
}

bool inline load_sequence_info(std::vector<SequenceInfo>& seq_infos, std::string url){
  if (boost::filesystem::exists(url.c_str())) {
    std::string filename(url.c_str());
    boost::iostreams::stream < boost::iostreams::file_source > file(
        filename.c_str());
    std::string line;
    while (std::getline(file, line)) {
      std::vector < std::string > strs;
      boost::split(strs, line, boost::is_any_of(" "));

      SequenceInfo info;
      info.path = strs[0];
      info.frame_count = boost::lexical_cast<int>(strs[1]);
      seq_infos.push_back(info);
    }
    return true;
  }
  return false;
}

/** The function takes the path to image index file and loads the whole information
 into a vector of Annotation structure.
 The information contain url of the image, bounding box of the person and
 coordinates of the body joints.
**/
bool inline load_annotations(std::vector<Annotation>& annotations, std::string url, int cluster_id = -1) {
  if (boost::filesystem::exists(url.c_str())) {
    std::string filename(url.c_str());
    boost::iostreams::stream < boost::iostreams::file_source > file(
        filename.c_str());
    std::string line;

//    int max_width = std::numeric_limits<int>::min();
//    int max_height = std::numeric_limits<int>::min();

    while (std::getline(file, line)) {
      std::vector < std::string > strs;
      boost::split(strs, line, boost::is_any_of(" "));

      Annotation ann;

      ann.url = strs[0];

      if (!boost::filesystem::exists(ann.url)){
        continue;
      }

      ann.bbox.x = boost::lexical_cast<int>(strs[1]);
      ann.bbox.y = boost::lexical_cast<int>(strs[2]);
      ann.bbox.width = boost::lexical_cast<int>(strs[3]);
      ann.bbox.height = boost::lexical_cast<int>(strs[4]);

//      cv::Mat img = cv::imread(ann.url);
//      max_width  = std::max(img.cols, max_width);
//      max_height = std::max(img.rows, max_height);

      ann.cluster_id = boost::lexical_cast<int>(strs[5]);

      if(cluster_id >= 0){
        if(cluster_id != ann.cluster_id){
          continue;
        }
      }
      int num_points = boost::lexical_cast<int>(strs[6]);
      ann.parts.resize(num_points);
      for (int i = 0; i < num_points; i++) {
        ann.parts[i].x = boost::lexical_cast<int>(strs[7 + 2 * i]);
        ann.parts[i].y = boost::lexical_cast<int>(strs[8 + 2 * i]);
      }
      annotations.push_back(ann);
    }
    return true;
  }
  std::cout << "file not found: " << url << std::endl;
  return false;
}

/** loads the annotations when we have multiple persons per image **/
bool inline load_multi_annotations(std::vector<MultiAnnotation>& m_annotations, std::string url, int cluster_id = -1) {

  Config ann_file;
  try{
    ann_file.readFile(url.c_str());
  }
  catch(const FileIOException &fioex){
    std::cerr << "Could not read annotation file: " << url << std::endl;
    return false;
  }
  catch(const ParseException &pex) {
    std::cerr << "Parse error: "<< pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return false;
  }

  // look up parameter settings / values
  try{

    const Setting &image_files = ann_file.getRoot()["image_files"];
    int num_images = image_files.getLength();
    for(int i=0; i<num_images; i++) {
      MultiAnnotation m_ann;

      m_ann.id = (int)image_files[i]["id"];
      m_ann.url = (const char*)image_files[i]["url"];

      const Setting &bbox = image_files[i]["bbox"];
      const Setting &cluster_id = image_files[i]["cluster_id"];
      const Setting &num_parts = image_files[i]["num_parts"];
      const Setting &parts = image_files[i]["parts"];
      const Setting &visibiliy_flags = image_files[i]["is_visible"];
      const Setting &scale = image_files[i]["scale"];
      const Setting &position = image_files[i]["position"];

      int num_persons = bbox.getLength();
      m_ann.count = num_persons;

      for(unsigned int n=0; n<num_persons; n++){
        Annotation ann;

        // url
        ann.url = m_ann.url;

        // bounding boxes
        ann.bbox.x      = bbox[n][0];
        ann.bbox.y      = bbox[n][1];
        ann.bbox.width  = bbox[n][2];
        ann.bbox.height = bbox[n][3];

        //cluster_id
        ann.cluster_id = cluster_id[0][n];

        // scale
        ann.scale = scale[0][n];


        unsigned int nparts = num_parts[0][n];

        // occlusion flags
        ann.is_visible.resize(nparts);
        for(unsigned int p=0; p<nparts; p++){
          ann.is_visible[p] = visibiliy_flags[n][p];
        }

        // parts
        ann.parts.resize(nparts, cv::Point(-1,-1));
        for(unsigned int p=0; p<nparts; p++){
          //if(ann.is_visible[p]){
          if(1){
            ann.parts[p].x = parts[n][2 * p];
            ann.parts[p].y = parts[n][2 * p + 1];
            ann.parts[p].x--;
            ann.parts[p].y--;
          }
        }
        ann.position = cv::Point_<int>(position[n][0], position[n][1]);

        // add to multi annotation structure.
        m_ann.persons.push_back(ann);
      }

      if(m_ann.persons.size()){
        if(image_files[i].exists("single_person")){
          const Setting &single_person = image_files[i]["single_person"];
          int sp_count = single_person[0].getLength();
          m_ann.single_person.resize(sp_count);
          for(unsigned int n=0; n<sp_count; n++){
            m_ann.single_person[n] = single_person[0][n];
          }
        }
        if(image_files[i].exists("person_groups")){
          const Setting &person_groups = image_files[i]["person_groups"];
          int sp_count = person_groups[0].getLength();
          m_ann.person_groups.resize(sp_count);
          for(unsigned int n=0; n<sp_count; n++){
            m_ann.person_groups[n] = person_groups[0][n];
          }
        }
        m_annotations.push_back(m_ann);
      }
    }
  }
  catch(const SettingNotFoundException &nfex) {
    std::cerr << "Not found in configuration file!" << std::endl;
    return false;
  }
  return true;
}

/** loads the annotations when we have multiple persons per image **/
bool inline load_multi_annotations_simple(std::vector<MultiAnnotation>& m_annotations, std::string url, int cluster_id = -1) {

  if (boost::filesystem::exists(url.c_str())) {
    std::string filename(url.c_str());
    boost::iostreams::stream < boost::iostreams::file_source > file(
        filename.c_str());
    std::string line;

//    int max_width = std::numeric_limits<int>::min();
//    int max_height = std::numeric_limits<int>::min();

    while (std::getline(file, line)) {
      std::vector < std::string > strs;
      boost::split(strs, line, boost::is_any_of(" "));

      MultiAnnotation m_ann;

      m_ann.url = strs[0];

      if (!boost::filesystem::exists(m_ann.url)){
        continue;
      }

      int num_points = boost::lexical_cast<int>(strs[2]);

      int num_persons = (strs.size() - 2) / (2*num_points);

      for(int p = 0; p < num_persons; p++){
        Annotation ann;
        ann.url = strs[0];
        ann.parts.resize(num_points);
        for (int i = 0; i < num_points; i++) {
          ann.parts[i].x = boost::lexical_cast<int>(strs[3 + 2 * i + 2*num_points*p]);
          ann.parts[i].y = boost::lexical_cast<int>(strs[4 + 2 * i + 2*num_points*p]);
        }
        m_ann.persons.push_back(ann);
      }
      m_annotations.push_back(m_ann);
    }
    return true;
  }
  std::cout << "file not found: " << url << std::endl;
  return false;
}


int inline get_upperbody_size( const std::vector<cv::Point> parts, body_pose::BodyPoseTypes pose_type ){

  cv::Point hip_center, shoulder_center;

  if(pose_type == body_pose::FULL_BODY_J13 || pose_type == body_pose::UPPER_BODY_J9){

    if( parts[4].x < 0 ||
        parts[3].x < 0 ||
        parts[2].x < 0 ||
        parts[1].x < 0) {
      return -1;
    }
    hip_center.x = (parts[4].x+ parts[3].x) /2;
    hip_center.y = (parts[4].y+ parts[3].y) /2;
    shoulder_center.x = (parts[2].x+ parts[1].x) /2;
    shoulder_center.y = (parts[2].y+ parts[1].y) /2;
  }
  else if(pose_type == body_pose::FULL_BODY_J14){
    if( parts[5].x < 0 ||
        parts[4].x < 0 ||
        parts[3].x < 0 ||
        parts[2].x < 0) {
      return -1;
    }
    hip_center.x = (parts[5].x+ parts[4].x) /2;
    hip_center.y = (parts[5].y+ parts[4].y) /2;
    shoulder_center.x = (parts[3].x+ parts[2].x) /2;
    shoulder_center.y = (parts[3].y+ parts[2].y) /2;
  }
  else if(pose_type == body_pose::FULL_BODY_J17){
      if( parts[8].x < 0 ||
        parts[7].x < 0 ||
        parts[4].x < 0 ||
        parts[3].x < 0) {
      return -1;
    }
    hip_center.x = (parts[7].x+ parts[8].x) /2;
    hip_center.y = (parts[7].y+ parts[8].y) /2;
    shoulder_center.x = (parts[4].x+ parts[3].x) /2;
    shoulder_center.y = (parts[4].y+ parts[3].y) /2;
  }

  return std::sqrt( (hip_center.x-shoulder_center.x)*(hip_center.x-shoulder_center.x) +
      (hip_center.y-shoulder_center.y)*(hip_center.y-shoulder_center.y) );
}

bool inline is_valid(cv::Point pt)
{
  if(pt.x >= 0 && pt.y >= 0)
    return true;
  else
    return false;
}

//void inline get_limb_sizes(Annotation ann, std::vector<int>& selected_parts, std::vector<int>& parents,
//               std::vector<int>& limb_sizes)
//{
//  for(unsigned int idx = 0; idx < selected_parts.size(); idx++){
//    int part_id = selected_parts[idx];
//    int parent = parents[part_id];
//    cv::Point parent_loc = ann.parts[parent];
//    cv::Point part_loc = ann.parts[part_id];
//    int size = ceil(std::sqrt( (parent_loc.x-part_loc.x)*(parent_loc.x-part_loc.x) +
//      (parent_loc.y-part_loc.y)*(parent_loc.y-part_loc.y) ));
//    limb_sizes.push_back(size);
//  }
//}
//
//
//float inline get_body_scale_factor(const Annotation ann){
//
//  body_pose::BodyPoseTypes pose_type = body_pose::FULL_BODY_J13;
//
//  std::vector<int> selected_parts;
//  selected_parts += 1,2,3,4,5,6,7,8,9,10,11,12;
//
//  std::vector<int> norm_sizes;
//  norm_sizes  += 23,23,50,50,21,21,21,21,34, 34, 32, 32;
//
//  std::vector<int> parents;
//  get_joint_constalation(parents, pose_type);
//
//  std::vector<int> limb_sizes;
//  std::vector<float> scales(norm_sizes.size());
//
//  get_limb_sizes(ann, selected_parts, parents, limb_sizes);
//
//  for(unsigned int i=0; i<limb_sizes.size(); i++){
//    scales[i] = static_cast<float>(norm_sizes[i])/limb_sizes[i];
//  }
//
//  std::sort(scales.begin(), scales.end());
//
//  float scale = scales[scales.size()/2];
//
//  return scale;
//}




void inline get_limb_sizes(Annotation ann, std::vector<int>& selected_parts, std::vector<int>& parents,
               std::vector<int>& limb_sizes)
{
  for(unsigned int idx = 0; idx < selected_parts.size(); idx++){
    int part_id = selected_parts[idx];
    int parent = parents[part_id];
    cv::Point parent_loc = ann.parts[parent];
    cv::Point part_loc = ann.parts[part_id];

    int size = 0;
    if(part_loc.x < 0 || part_loc.y < 0 ||
       parent_loc.x < 0 || parent_loc.y < 0){
       size = -1;
    }else {
       size = ceil(std::sqrt( (parent_loc.x-part_loc.x)*(parent_loc.x-part_loc.x) +
            (parent_loc.y-part_loc.y)*(parent_loc.y-part_loc.y) ));
    }
    limb_sizes.push_back(size);
  }
}


float inline get_body_scale_factor(const Annotation ann,
      std::vector<int> selected_parts, std::vector<int> norm_sizes,
      body_pose::BodyPoseTypes pose_type){

//  std::vector<int> selected_parts;
//  selected_parts += 1,2,3,4,5,6,7,8,9,10,11,12;

  //std::vector<int> norm_sizes;
  //norm_sizes  += 23,23,50,50,21,21,21,21,34, 34, 32, 32;

  std::vector<int> parents;
  get_joint_constalation(parents, pose_type);

  std::vector<int> limb_sizes;
  std::vector<float> scales;

  get_limb_sizes(ann, selected_parts, parents, limb_sizes);

  for(unsigned int j=0; j<limb_sizes.size(); j++){
    if(limb_sizes[j] > 0){
      scales.push_back(static_cast<float>(norm_sizes[j])/limb_sizes[j]);
    }
  }

  std::sort(scales.begin(), scales.end());

  float scale = 0;
  if(scales.size() >= 3){
    scale = scales[scales.size()/2];
  }
  else{
    scale = 1;
  }

  return scale;
}


int inline get_upperbody_size( const Annotation ann, body_pose::BodyPoseTypes pose_type ){
  return get_upperbody_size(ann.parts, pose_type);
}

int inline get_upperbody_size_temporal( const std::vector<cv::Point> parts, int nNeighbours){

  int img_count = nNeighbours*2+1;

  for(int idx = 0; idx < img_count; idx++){
    if( parts[idx*13+4].x < 0 ||
        parts[idx*13+3].x < 0 ||
        parts[idx*13+2].x < 0 ||
        parts[idx*13+1].x < 0) {
          return -1;
      }
  }

  int body_size = 0;

  for(int idx = 0; idx < img_count; idx++){
    cv::Point hip_center;
    hip_center.x = (parts[idx*13+4].x+ parts[idx*13+3].x) /2;
    hip_center.y = (parts[idx*13+4].y+ parts[idx*13+3].y) /2;
    cv::Point shoulder_center;
    shoulder_center.x = (parts[idx*13+2].x+ parts[idx*13+1].x) /2;
    shoulder_center.y = (parts[idx*13+2].y+ parts[idx*13+1].y) /2;

    body_size += std::sqrt( (hip_center.x-shoulder_center.x)*(hip_center.x-shoulder_center.x) +
      (hip_center.y-shoulder_center.y)*(hip_center.y-shoulder_center.y) );
  }

  body_size /= img_count; // average body size of all frames

  return body_size;
}

bool inline get_symmetric_parts(std::vector<int>& symmetric_parts, body_pose::BodyPoseTypes pose_type)
{
  symmetric_parts.clear();
  if(pose_type == body_pose::FULL_BODY_J13){
    symmetric_parts += 0,2,1,4,3,7,8,5,6,11,12,9,10;
  }
  else if(pose_type == body_pose::FULL_BODY_J14){
    symmetric_parts += 0,1,3,2,5,4,8,9,6,7,12,13,10,11;
  }
  return true;
}

bool inline load_bounding_boxes(std::string url,
            std::vector<std::vector<cv::Rect> >& bboxes,
            std::vector<std::vector<float> >& scores)
{

  if (boost::filesystem::exists(url.c_str())) {
    std::string filename(url.c_str());
    boost::iostreams::stream < boost::iostreams::file_source > file(
        filename.c_str());
    std::string line;
    bboxes.clear();

    while (std::getline(file, line)) {
      std::vector < std::string > strs;
      boost::split(strs, line, boost::is_any_of(" "));



      int num_persons = boost::lexical_cast<int>(strs[0]);

      std::vector<cv::Rect> i_bboxes;
      std::vector<float> i_scores;
      for(size_t i=0; i<num_persons; i++){
        int x1 = boost::lexical_cast<int>(strs[i*5+1])-1;
        int y1 = boost::lexical_cast<int>(strs[i*5+2])-1;
        int x2 = boost::lexical_cast<int>(strs[i*5+3])-1;
        int y2 = boost::lexical_cast<int>(strs[i*5+4])-1;
        float score = boost::lexical_cast<float>(strs[i*5+5]);

        cv::Rect b(x1,y1, x2-x1+1, y2-y1+1);
        i_bboxes.push_back(b);
        i_scores.push_back(score);
      }

      bboxes.push_back(i_bboxes);
      scores.push_back(i_scores);
    }
    return true;
  }
  std::cout << "file not found: " << url << std::endl;
  return false;
}

bool inline load_image_urls(std::string url, std::vector<std::string>& image_urls)
{

  if (boost::filesystem::exists(url.c_str())) {
    std::string filename(url.c_str());
    boost::iostreams::stream < boost::iostreams::file_source > file(
        filename.c_str());
    std::string line;
    image_urls.clear();

    while (std::getline(file, line)) {
        image_urls.push_back(line);
    }
    return true;
  }
  std::cout << "file not found: " << url << std::endl;
  return false;
}


#endif /* COMMON_HPP_ */
