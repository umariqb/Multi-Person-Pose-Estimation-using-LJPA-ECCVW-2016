/*
 * cnn_features.hpp
 *
 *  Created on:  September 18, 2015
 *      Author:  Umar Iqbal
 */


#ifndef CNN_FEATURES_HPP
#define CNN_FEATURES_HPP

#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include "caffe/caffe.hpp"
#include "caffe/util/io.hpp"
#include "caffe/blob.hpp"
#include "cpp/vision/features/cnn/caffe_utils.hpp"
#include "cpp/utils/system_utils.hpp"

using namespace caffe;
using boost::shared_ptr;

namespace vision
{
namespace features
{

class CNNFeatures
{
  public:

    CNNFeatures(std::string config_file);


    bool init(const std::string pretrained_net_proto,
                const std::string feature_extraction_proto,
                const std::string mean_file = "",
                bool use_gpu = true, int device_id = 0);

    cv::Mat preprocess(cv::Mat img,  bool use_mean_pixel = false,
              bool use_original_image_size = false);

    bool extract(const cv::Mat, const std::vector<string> feat_names,
                std::vector<cv::Mat_<float> >& features,
                bool resize_to_img_size = false,
                bool use_original_image_size = false,
                int num_threads = -1);

    // extract cpm features
    bool extract_cpm(const cv::Mat, const std::vector<string> feat_names,
                std::vector<cv::Mat_<float> >& features,
                cv::Point_<int> center,
                bool resize_to_img_size = false,
                bool use_original_image_size = false,
                int num_threads = -1);


    bool extract_and_save(const cv::Mat image,
                const std::vector<std::string> feat_names,
                std::vector<cv::Mat_<float> >& features,
                bool resize_to_img_size, std::string path);

    bool load_features(std::vector<cv::Mat>& features, const std::string path);

    bool get_caffe_blobs(const cv::Mat image,
               const std::vector<string> feat_names,
               std::vector<boost::shared_ptr<Blob<float> > >& feature_blobs,
               bool use_original_image_size);

    bool extract_all_and_save(const std::vector<cv::Mat> images,
                const std::vector<std::string> img_names,
                const std::vector<std::string> feat_names,
                std::string dir_path,
                std::string database_type = "lmdb");

    bool set_mean_pixel(cv::Scalar mc);
    cv::Scalar get_mean_pixel();

    cv::Size get_input_geometry();

    virtual ~CNNFeatures();
  protected:


  private:

    bool WrapInputLayer(std::vector<cv::Mat>& input_channels);
    boost::shared_ptr<Net<float> > net;
    cv::Size input_geometry;
    int num_channels;
    cv::Mat mean;
    cv::Scalar channel_mean;
};

cv::Mat get_center_map(cv::Size s, float sigma, cv::Point center);
void get_center_map(cv::Mat &m, cv::Size s, float sigma, cv::Point center);


} /* namespace features */
} /* namespace vision */

#endif // CNN_FEATURES_HPP

