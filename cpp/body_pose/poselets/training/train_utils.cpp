/*
 * train_utils.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: mdantone
 */

#include "train_utils.hpp"
#include "cpp/vision/geometry_utils.hpp"
#include "cpp/vision/opencv_utils.hpp"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace vision::geometry_utils;
using namespace vision::opencv_utils;

namespace body_pose {
namespace poselets {


void crop_images(vector<Annotation>& annotations,
                 const vector<Mat>& images,
                 vector<Mat>& cropped_images,
                 float scale_factor) {

  // calculate bbox size;
  Rect bbox(0,0,0,0);
  for(int i=0; i < annotations.size(); i++) {
    const Annotation& ann = annotations[i];
    Rect tmp = get_bbox_containing_points(ann.parts);

    bbox.width = max(tmp.width, bbox.width);
    bbox.height = max(tmp.height, bbox.height);
  }

  bbox.width *= scale_factor;
  bbox.height *= scale_factor;

  // crop the actual image
  for(int i=0; i < annotations.size(); i++) {
    const Annotation& ann = annotations[i];
    Rect tmp = get_bbox_containing_points(ann.parts);
    Point center(tmp.x+tmp.width/2, tmp.y+tmp.height/2 );

    Rect crop_bbox = bbox;
    crop_bbox.x = center.x - crop_bbox.width/2;
    crop_bbox.y = center.y - crop_bbox.height/2;



    Mat img = extract_roi(images[i], crop_bbox);
    cropped_images.push_back(img);
  }
}

void calculate_box(vector<Annotation>& annotations,
                 const vector<int>& part_indicies,
                 float scale_factor, int default_size) {
  if(part_indicies.size() == 1) {
    int part_id = part_indicies[0];
    for(int i=0; i < annotations.size(); i++) {
      const Annotation& ann = annotations[i];

      Rect bbox = Rect(ann.parts[part_id].x - default_size / 2,
                       ann.parts[part_id].y - default_size / 2,
                       default_size,
                       default_size);
      annotations[i].bbox = bbox;
    }
  }else{


    // calculate bbox size;
    Rect bbox(0,0,0,0);
    for(int i=0; i < annotations.size(); i++) {
      Annotation& ann = annotations[i];

      vector<Point> parts;
      for(int j=0; j < part_indicies.size(); j++) {
        parts.push_back(ann.parts[part_indicies[j] ]);
      }
      annotations[i].bbox = get_bbox_containing_points(parts);
    }
  }
}

void calculate_poselet_bbox(vector<Annotation>& annotations,
                 const vector<int>& part_indicies,
                 float scale_factor, int default_size) {

  if(part_indicies.size() == 1) {
    int part_id = part_indicies[0];
    for(int i=0; i < annotations.size(); i++) {
      const Annotation& ann = annotations[i];

      Rect bbox = Rect(ann.parts[part_id].x - default_size / 2,
                       ann.parts[part_id].y - default_size / 2,
                       default_size,
                       default_size);

      annotations[i].bbox = bbox;
    }
  }else{



    vector<int> widths, heights;
    for(int i=0; i < annotations.size(); i++) {
      const Annotation& ann = annotations[i];
      vector<Point> parts;
      for(int j=0; j < part_indicies.size(); j++) {
        parts.push_back(ann.parts[part_indicies[j] ]);
      }
      Rect tmp = get_bbox_containing_points(parts);
      widths.push_back(tmp.width);
      heights.push_back(tmp.height);

    }

    std::sort(widths.begin(), widths.end() );
    std::sort(heights.begin(), heights.end() );

    // calculate bbox size;
    int index = widths.size()*0.95;
    Rect bbox(0,0,widths[index],heights[index]);

    bbox.width *= scale_factor;
    bbox.height *= scale_factor;

    bbox.width = std::max(default_size, bbox.width);
    bbox.height = std::max(default_size, bbox.height);

//    float size = 16;
//    LOG(INFO) << "width: " << bbox.width << " -> " << std::max(double(1.0), round(bbox.width/size) ) * size;
//    LOG(INFO) << "height: " << bbox.height << " -> " << std::max(double(1.0), round(bbox.height/size) ) * size;
//    bbox.width = std::max(double(1.0), round(bbox.width/size) ) * size;
//    bbox.height = std::max(double(1.0), round(bbox.height/size) ) * size;


    // crop the actual image
    for(int i=0; i < annotations.size(); i++) {
      const Annotation& ann = annotations[i];

      vector<Point> parts;
      for(int j=0; j < part_indicies.size(); j++) {
        parts.push_back(ann.parts[part_indicies[j] ]);
      }

      Rect tmp = get_bbox_containing_points(parts);
      Point center(tmp.x+tmp.width/2, tmp.y+tmp.height/2 );

      Rect crop_bbox = bbox;
      crop_bbox.x = center.x - crop_bbox.width/2;
      crop_bbox.y = center.y - crop_bbox.height/2;
      annotations[i].bbox = crop_bbox;

      if(false) {

        Mat img = cv::imread( annotations[i].url );
        rectangle(img, tmp, cv::Scalar(255, 255, 255, 0));
        rectangle(img, annotations[i].bbox, cv::Scalar(255, 0, 255, 0));

        imshow("img", img);
        waitKey(0);
      }


    }

  }
}



} /* namespace poselets */
} /* namespace body_pose */
