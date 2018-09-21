

#include "cpp/learning/SPLP/utils.hpp"
#include "cpp/learning/SPLP/detection.hpp"
#include "cpp/vision/geometry_utils.hpp"
#include "cpp/utils/string_utils.hpp"

using namespace cv;
using namespace std;

namespace learning
{

namespace splp
{

cv::Mat compute_spatial_relation_feature(const Detection& d1, const Detection& d2, float patch_size)
{
  cv::Mat feat = cv::Mat::zeros(1, 6, CV_32F);

  float h1 = d1.scale * patch_size;
  float h2 = d2.scale * patch_size;
  float h = (h1 + h2)/2;

  feat.at<float>(0,0) = std::abs(d1.loc.x - d2.loc.x)/h;
  feat.at<float>(0,1) = std::abs(d1.loc.y - d2.loc.y)/h;
  feat.at<float>(0,2) = std::abs(h1 - h2)/h;

  cv::Rect r1 = cv::Rect(d1.loc.x-h1/2, d1.loc.y-h1/2, h1, h1);
  cv::Rect r2 = cv::Rect(d1.loc.x-h2/2, d1.loc.y-h2/2, h2, h2);

  feat.at<float>(0,3) = vision::geometry_utils::intersection_over_union(r1, r2);
  feat.at<float>(0,4) = vision::geometry_utils::intersection_area(r1,r2)/static_cast<float>(std::min(h1*h1, h2*h2));
  feat.at<float>(0,5) = vision::geometry_utils::intersection_area(r1,r2)/static_cast<float>(std::max(h1*h1, h2*h2));

  cv::Mat quad_feat;
  cv::pow(feat, 2, quad_feat);

  cv::Mat exp_feat;
  cv::exp(feat, exp_feat);

  hconcat(feat, quad_feat, feat);
  hconcat(feat, exp_feat, feat);

  return feat;
}


cv::Mat compute_spatial_relation_feature(const cv::Point pt1, const cv::Point pt2, float patch_size){

  cv::Mat feat = cv::Mat::zeros(1, 6, CV_32F);

  feat.at<float>(0,0) = std::abs(pt1.x - pt2.x)/static_cast<float>(patch_size);
  feat.at<float>(0,1) = std::abs(pt1.y - pt2.y)/static_cast<float>(patch_size);
  feat.at<float>(0,2) = 0;

  cv::Rect r1 = cv::Rect(pt1.x-patch_size/2, pt1.y-patch_size/2, patch_size, patch_size);
  cv::Rect r2 = cv::Rect(pt2.x-patch_size/2, pt2.y-patch_size/2, patch_size, patch_size);

  feat.at<float>(0,3) = vision::geometry_utils::intersection_over_union(r1, r2);
  feat.at<float>(0,4) = vision::geometry_utils::intersection_area(r1,r2)/(patch_size*patch_size);
  feat.at<float>(0,5) = vision::geometry_utils::intersection_area(r1,r2)/(patch_size*patch_size);

  cv::Mat quad_feat;
  cv::pow(feat, 2, quad_feat);

  cv::Mat exp_feat;
  cv::exp(-1*feat, exp_feat);

  hconcat(feat, quad_feat, feat);
  hconcat(feat, exp_feat, feat);

  return feat;
}

cv::Mat extract_pairwise_feat(cv::Point& p1, cv::Point& p2){
  cv::Mat feat = cv::Mat::zeros(1,4, CV_32F);
  feat.at<float>(0,0) = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
  cv::Point offset = p1 - p2;
  feat.at<float>(0,1) = atan2(offset.y, offset.x);
  feat.at<float>(0,2) = offset.x;
  feat.at<float>(0,3) = offset.y;
  return feat;
}

bool extract_pairwise_feat_from_offsets(std::vector<cv::Point> offsets, cv::Mat& feat)
{

  if(offsets.size() < 1){
    std::cerr<<"No offsets provided."<<std::endl;
    return false;
  }

  feat = cv::Mat::zeros(offsets.size(),6,CV_32F);

  for(unsigned int i=0; i<offsets.size(); i++){
    feat.at<float>(i,0) = sqrt(pow(offsets[i].x, 2) + pow(offsets[i].y, 2));
    feat.at<float>(i,1) = atan2(offsets[i].y, offsets[i].x);
    feat.at<float>(i,2) = offsets[i].x;
    feat.at<float>(i,3) = offsets[i].y;
    feat.at<float>(i,4) = -offsets[i].x;
    feat.at<float>(i,5) = -offsets[i].y;
  }

  return true;
}


bool extract_pairwise_feat_from_offset(cv::Point offset, cv::Mat& feat)
{
  feat = cv::Mat::zeros(1, 6,CV_32F);
  feat.at<float>(0,0) = offset.x;
  feat.at<float>(0,1) = offset.y;
  feat.at<float>(0,2) = -offset.x;
  feat.at<float>(0,3) = -offset.y;
  feat.at<float>(0,4) = sqrt(pow(offset.x, 2) + pow(offset.y, 2));
  feat.at<float>(0,5) = atan2(offset.y, offset.x);

  return true;
}


cv::Mat shuffleRows(const cv::Mat &matrix)
{
  std::vector <int> seeds;
  for (int cont = 0; cont < matrix.rows; cont++)
    seeds.push_back(cont);

  std::random_shuffle(seeds.begin(), seeds.end());

  cv::Mat output;
  for (int cont = 0; cont < matrix.rows; cont++)
    output.push_back(matrix.row(seeds[cont]));

  return output;
}


void normalize_data_range(const cv::Mat& data, cv::Mat& norm_data, cv::Mat& means, cv::Mat& stds)
{
  stds = Mat(1, data.cols, CV_32FC1);
  means = Mat(1, data.cols, CV_32FC1);

  for (int i = 0; i < data.cols; i++){
    Scalar mean, std;
    cv::meanStdDev(data.col(i), mean, std);
    stds.at<float>(i) = static_cast<float>(std.val[0]);
    means.at<float>(i) = static_cast<float>(mean.val[0]);
  }

  cv::Mat means_mat, stds_mat;
  repeat(means, data.rows, 1, means_mat);
  repeat(stds, data.rows, 1, stds_mat);

  stds += 1e-6; // avoid NaNs

  subtract(data, means_mat, norm_data);
  divide(norm_data, 2*stds_mat, norm_data);
}

void normalize_data(cv::Mat data, cv::Mat dst,  cv::Mat mean, cv::Mat std){
  subtract(data, mean, dst);
  divide(dst, 2*std, dst);
}

void nms(vector<learning::splp::Detection>& src, vector<learning::splp::Detection>& dst, float thresh)
{
  if(!src.size()) return;

  Mat_<float> scores = Mat::zeros(1, src.size(), CV_32F);
  for(unsigned int dIdx=0; dIdx<src.size(); dIdx++){
    scores.at<float>(0, dIdx)= src[dIdx].score;
  }

  Mat sorted_idx;
  sortIdx(scores, sorted_idx, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);

  const int* p = sorted_idx.ptr<int>(0);
  vector<int> I(p, p+sorted_idx.cols);
  vector<int> pick;

  while(I.size()){
    int last = I.size()-1;
    int i = I[last];
    pick.push_back(i);

    vector<bool> suppress(I.size(), false);
    suppress[last] = true;

    for(int pos=0; pos<last; pos++){
      int j = I[pos];
      Point pt1 = src[i].loc;
      Point pt2 = src[j].loc;

      float dist = sqrt(pow(pt1.x-pt2.x, 2) + pow(pt1.y - pt2.y, 2));
      if(dist <= thresh){
        suppress[pos] = true;
      }
    }
    std::vector<int> diff;
    for(unsigned int pos=0; pos<I.size(); pos++){
      if(!suppress[pos]){
        diff.push_back(I[pos]);
      }
    }
    I = diff;
  }

  dst.clear();
  dst.reserve(pick.size());
  for(unsigned int i=0; i<pick.size(); i++){
    dst.push_back(src[pick[i]]);
  }
  return;
}



void draw_detections(cv::Mat image, std::vector<learning::splp::Detection>& detections, bool draw_lines)
{
  Mat plot = image.clone();
  vector<Scalar> colors;
  colors.push_back(Scalar(0,255,0));
  colors.push_back(Scalar(0,128,0));
  colors.push_back(Scalar(128,0,0));
  colors.push_back(Scalar(255,0,0));
  colors.push_back(Scalar(0,128,0));
  colors.push_back(Scalar(0,255,0));
  colors.push_back(Scalar(0,0,128));
  colors.push_back(Scalar(0,0,255));
  colors.push_back(Scalar(255,0,128));
  colors.push_back(Scalar(255,0,255));
  colors.push_back(Scalar(255,128,0));
  colors.push_back(Scalar(255,255,0));
  colors.push_back(Scalar(128,255,0));
  colors.push_back(Scalar(255,255,0));
  colors.push_back(Scalar(128,0,255));
  colors.push_back(Scalar(255,0,255));
  colors.push_back(Scalar(0,128,255));

  for(unsigned int dIdx=0; dIdx<detections.size(); dIdx++){
    int label = detections[dIdx].label;
    circle(plot, detections[dIdx].loc, 3, colors[label], 2);

    if(true){
      string num = ::utils::num2str(dIdx);
      putText(plot, num, detections[dIdx].loc,  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255));
    }
  }

  if(draw_lines){
    for(unsigned int dIdx=0; dIdx<detections.size()-1; dIdx++){
      for(unsigned int ddIdx=dIdx+1; ddIdx<detections.size(); ddIdx++){
      line(plot, detections[dIdx].loc, detections[ddIdx].loc, Scalar(0,0,255));
      }
    }
  }

  imshow("detections", plot);
  waitKey(0);
}

}

}
