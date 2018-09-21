/*
 * cnn_features.cpp
 *
 *  Created on:  September 18, 2015
 *      Author:  Umar Iqbal
 */

#include "cpp/vision/features/cnn/cnn_features.hpp"
#include "cpp/vision/features/cnn/caffe_utils.hpp"
#include "cpp/utils/serialization/opencv_serialization.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/utils/timing.hpp"
#include "cpp/vision/features/cnn/cpm_params.hpp"


#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <libconfig.h++>

using namespace cv;
using namespace std;
using boost::shared_ptr;
using namespace libconfig;
using namespace boost::assign;

namespace vision
{
namespace features
{

CNNFeatures::CNNFeatures(std::string config_file){
    Config config;
    try{
      config.readFile(config_file.c_str());
    }

    catch(const FileIOException &fioex){
      LOG(ERROR)<< "I/O error while reading file." << std::endl;
    }
    catch(const ParseException &pex){
      LOG(ERROR) << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
    }

    const Setting &root = config.getRoot();

    if(! root.exists("cnn_params")){
      LOG(ERROR)<<"Cannot read CNN parameters from config file: "<<config_file;
    }

    Setting &cnn_params = root["cnn_params"];

    string pretrained_net_proto = (const char*)cnn_params["pretrained_net_proto"];
    string feature_extraction_proto = (const char*)cnn_params["feature_extraction_proto"];
    string mean_file = (const char*)cnn_params["mean_file"];
    bool use_gpu = (bool)cnn_params["use_gpu"];
    int device_id = (int)cnn_params["device_id"];
    init(pretrained_net_proto, feature_extraction_proto, mean_file, use_gpu, device_id);

    Scalar mean_pixel;
    mean_pixel.val[0] = (double)cnn_params["mean_pixel"][0][0];
    mean_pixel.val[1] = (double)cnn_params["mean_pixel"][0][1];
    mean_pixel.val[2] = (double)cnn_params["mean_pixel"][0][2];

    set_mean_pixel(mean_pixel);
}

bool CNNFeatures::init(const std::string pretrained_net_proto,
                         const std::string feature_extraction_proto,
                         const std::string mean_file,
                         bool use_gpu, int device_id)
{
  net.reset(new caffe::Net<float>(feature_extraction_proto, caffe::TEST));
  if (use_gpu) {
    LOG(INFO)<< "Using GPU";
    Caffe::SetDevice(device_id);
    Caffe::set_mode(Caffe::GPU);
  } else {
    LOG(ERROR) << "Using CPU";
    Caffe::set_mode(Caffe::CPU);
  }

  net->CopyTrainedLayersFrom(pretrained_net_proto);

  Blob<float>* input_layer = net->input_blobs()[0];
  num_channels = input_layer->channels();
  LOG(INFO)<<num_channels;
  CHECK(num_channels == 3 || num_channels == 1 || num_channels == 4)
    << "Input layer should have 1 or 3 or 4 channels.";
  input_geometry = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  if(mean_file != ""){
    mean = caffe::utils::GetMean(mean_file);
    mean.convertTo(mean, CV_32F);
    channel_mean = cv::mean(mean);
  }
  else{
    mean = cv::Mat::zeros(input_geometry, CV_32FC(num_channels));
    channel_mean = cv::Scalar(0,0,0,0);
  }
  return true;
}

cv::Mat CNNFeatures::preprocess(cv::Mat img,  bool use_mean_pixel, bool use_original_image_size)
{
  cv::Mat sample = img;

  // do mean normalization
  cv::Mat sample_normalized;
  cv::Mat sample_resized, sample_float;

  if(use_original_image_size){
    int width  = img.cols;
    int height = img.rows;

    width = (width % 4) == 1 ? width : (width - (width % 4) + 1);
    height = (height % 4) == 1 ? height : (height - (height % 4) + 1);
    input_geometry = cv::Size(width, height);
  }


  if(use_mean_pixel){  // if a single mean pixel has to be used
    if (sample.size() != input_geometry)
      cv::resize(sample, sample_resized, input_geometry, CV_INTER_CUBIC);
    else
      sample_resized = sample;

    sample_resized.convertTo(sample_float, CV_32F);
    cv::Mat mean_mat = cv::Mat(input_geometry, mean.type(), channel_mean);
    cv::subtract(sample_float, mean_mat, sample_normalized);
  }
  else{ // if complete mean image has to be used
    if (sample.size() != mean.size())
      cv::resize(sample, sample_resized, mean.size());
    else
      sample_resized = sample;

    sample_resized.convertTo(sample_float, CV_32F);
    cv::subtract(sample_float, mean, sample_normalized);

    cv::resize(sample_normalized, sample_normalized, input_geometry, CV_INTER_CUBIC);
  }
  return sample_normalized;
}

bool CNNFeatures::get_caffe_blobs(Mat image, vector<string> feat_names,
              std::vector<boost::shared_ptr<Blob<float> > >& feature_blobs,
              bool use_original_image_size)
{
    CHECK(feat_names.size());

    cv::Mat norm_image = image.clone();
//    norm_image = preprocess(image, true, use_original_image_size);

    int channels = norm_image.channels();
    int height = norm_image.rows;
    int width = norm_image.cols;

    // resize networks's input layer according to input image
    net->input_blobs()[0]->Reshape(1, channels, height, width);

    //wrap input layer with image
    vector<cv::Mat> input_channels;
    WrapInputLayer(input_channels);
    split(norm_image,input_channels);

    ///checks weather wraping is correct
    CHECK(reinterpret_cast<float*>(input_channels.at(0).data) ==
          net->input_blobs()[0]->cpu_data() )
          <<"Input channels are not wrapping the input layer";

    //forward pass
    net->Reshape();
    net->ForwardPrefilled();

    int num_feat = feat_names.size();

    feature_blobs.resize(num_feat);
    for(int i=0; i<num_feat; i++){
      feature_blobs[i] = net->blob_by_name(feat_names[i]);
    }
    return true;
}

bool extract_and_resize_mt(cv::Size img_sz, boost::shared_ptr<Blob<float> > feature_blob,
          vector<Mat_<float> >& features, bool resize_to_img_size, int start_idx)
{
  int height = feature_blob->height();
  int width = feature_blob->width();
  int channels = feature_blob->channels();
  int dim = width * height;

  for (int c=0; c<channels; c++){
    cv::Mat feat_mat(cv::Mat(height, width, CV_32FC1, feature_blob->mutable_cpu_data()+dim*c));
    if(resize_to_img_size){
      resize(feat_mat, feat_mat, img_sz, CV_INTER_CUBIC);
    }
    features[start_idx+c] = feat_mat;
  }
}

bool pad_around(Mat& image, int boxsize, Point_<int> center, int pad_value, Mat& img_padded, vector<int>& pad)
{
  int h = image.rows;
  int w = image.cols;

  pad.clear();
  pad.resize(4,0);

  pad[0] = boxsize/2 - center.y;
  pad[2] = boxsize/2 - (h-center.y);
  pad[1] = boxsize/2 - center.x;
  pad[3] = boxsize/2 - (w-center.x);

  Mat padded = image.clone();

  Scalar pad_val(pad_value, pad_value, pad_value);

  if(pad[0] > 0){
    Mat pad_up(pad[0], padded.cols, CV_8UC3, pad_val);
    vconcat(pad_up, padded, padded);
  }

  if(pad[1] > 0){
    Mat pad_left(padded.rows, pad[1], CV_8UC3, pad_val);
    hconcat(pad_left, padded, padded);
  }

  if(pad[2] > 0){
    Mat pad_down(pad[2], padded.cols, CV_8UC3, pad_val);
    vconcat(padded, pad_down, padded);
  }

  if(pad[3] > 0){
    Mat pad_right(padded.rows, pad[3], CV_8UC3, pad_val);
    hconcat(padded, pad_right, padded);
  }

  center = center + Point(max(0, pad[1]), max(0, pad[0]));

  Rect bbox;
  bbox.y      = center.y - (boxsize/2-1) - 1;
  bbox.height = center.y + boxsize/2 - bbox.y;
  bbox.x      = center.x - (boxsize/2-1) - 1;
  bbox.width  = center.x + boxsize/2 - bbox.x;

  padded = padded(bbox);

  img_padded = padded;

  return true;
}

static void meshgrid(const cv::Mat_<float> &xgv, const cv::Mat_<float> &ygv,
              cv::Mat_<float> &X, cv::Mat_<float> &Y)
{
  cv::repeat(xgv.reshape(1,1), ygv.total(), 1, X);
  cv::repeat(ygv.reshape(1,1).t(), 1, xgv.total(), Y);
}

void get_center_map(cv::Mat &m, Size s, float sigma, Point center){


}


cv::Mat get_center_map(Size s, float sigma, Point center){

  std::vector<float> t_x(s.width), t_y(s.height);
  std::iota(std::begin(t_x), std::end(t_x), 0);
  std::iota(std::begin(t_y), std::end(t_y), 0);

  Mat_<float> X, Y;
  meshgrid(cv::Mat(t_x), cv::Mat(t_y), X, Y);

  X = X - center.x;
  Y = Y - center.y;

  cv::pow(X,2,X);
  cv::pow(Y,2,Y);

  Mat D2 = X + Y;

  Mat Exponent = D2 / 2.0 / sigma / sigma;

  Mat label;
  cv::exp(-Exponent, label);

  return label;
}

cv::Mat preprocess_cpm(Mat image, float sigma, int boxsize)
{

  Mat out_image = image.clone();
  out_image.convertTo(out_image, CV_32F, 1.0/256);
  subtract(out_image, Scalar(0.5,0.5,0.5), out_image);

  Mat center_map = get_center_map(Size(boxsize, boxsize), sigma, Point(boxsize/2, boxsize/2));
  vector<Mat> spl;
  split(out_image, spl);
  spl.push_back(center_map);

  merge(spl, out_image);
  return out_image;
}

Mat resize_into_scaled_img(const Mat_<float>& score, vector<int>& pad, float shift = 0)
{

  CHECK(pad.size());

  Mat out_score = score.clone();
  if(pad[0] < 0){
    Mat padup = Mat::zeros(-pad[0], score.cols, CV_32FC1) + shift;
    vconcat(padup, out_score, out_score);
  }
  else{
    out_score = out_score.rowRange(pad[0], out_score.rows);
  }

  if(pad[1] < 0){
    Mat padleft = Mat::zeros(out_score.rows, -pad[1], CV_32FC1) + shift;
    hconcat(padleft, out_score, out_score);
  }
  else{
    out_score = out_score.colRange(pad[1], out_score.cols);
  }

  if(pad[2] < 0){
    Mat paddown = Mat::zeros(-pad[2], out_score.cols, CV_32FC1) + shift;
    vconcat(out_score, paddown, out_score);
  }
  else{
    out_score = out_score.rowRange(0, out_score.rows - pad[2]);
  }
  if(pad[3] < 0){
    Mat padright = Mat::zeros(out_score.rows, -pad[3], CV_32FC1) + shift;
    hconcat(out_score, padright, out_score);
  }
  else{
    out_score = out_score.colRange(0, out_score.cols-pad[3]);
  }

  return out_score;
}

void resize_into_scaled_img(vector<Mat_<float> >& scores, vector<int>& pad)
{
  for(size_t i = 0; i<scores.size()-1; i++){
    scores[i] = resize_into_scaled_img(scores[i], pad, 0);
  }
  scores[scores.size()-1] = resize_into_scaled_img(scores[scores.size()-1], pad, 1);
}

bool CNNFeatures::extract_cpm(const Mat image, vector<string> feat_names,
          vector<Mat_<float> >& features, Point_<int> center, bool resize_to_img_size,
          bool use_original_image_size, int num_threads)
{

    CPMParam params;
    params.boxsize  = 368;
    params.stages   = 6;
    params.padvalue = 128;
    params.sigma = 21;

    vector<int> order_to_cpm;
    order_to_cpm += 0,1,3,8,9,2,6,7,5,12,13,4,10,11,14;

    vector<int> scales;
    float start_scale = 0.7;
    float end_scale = 1.4;

    vector<Mat_<float> > sum;
    int scale_count = 0;
    for(float scale=start_scale; scale <= end_scale; scale=scale+0.1)
    {
      scale_count++;
      Mat test_image = image.clone();

      if(scale > 1){
        cv::resize(test_image, test_image, Size(0,0), scale, scale, CV_INTER_CUBIC);
      }
      else{
        cv::resize(test_image, test_image, Size(0,0), scale, scale, CV_INTER_AREA);
      }

      Point center_s = center * scale;

      vector<int> pad;
      pad_around(test_image, params.boxsize, center_s, params.padvalue, test_image, pad);
      test_image = preprocess_cpm(test_image, params.sigma, params.boxsize);

      vector<Mat_<float> > scores;
      extract(test_image, feat_names, scores, resize_to_img_size, false, num_threads); // forward pass

      // unpad scores
      resize_into_scaled_img(scores, pad);

      // resize to original image size
      for(size_t i=0; i<scores.size(); i++){
        if(scores[i].rows * scores[i].cols < image.rows * image.cols){
          resize(scores[i], scores[i], image.size(), 0, 0, CV_INTER_CUBIC);
        }
        else{
          resize(scores[i], scores[i], image.size(), 0, 0, CV_INTER_AREA);
        }
      }

      if(sum.size() == 0){
        sum = scores;
      }
      else{
        for(size_t i=0; i<scores.size(); i++){
          add(sum[i], scores[i], sum[i]);
        }
      }
    }

    features.resize(sum.size());
    for(size_t i=0; i<sum.size(); i++){
      sum[i] /= static_cast<float>(scale_count);
    }

    for(size_t i=0; i<sum.size(); i++){
      features[order_to_cpm[i]] = sum[i];
    }
    return true;
}


bool CNNFeatures::extract(Mat image, vector<string> feat_names,
          vector<Mat_<float> >& features, bool resize_to_img_size,
          bool use_original_image_size, int num_threads)
{
    std::vector<boost::shared_ptr<Blob<float> > > feature_blobs;
    get_caffe_blobs(image, feat_names, feature_blobs, use_original_image_size);
    CHECK_EQ(feat_names.size(), feature_blobs.size());

    int total_channels = 0;
    vector<int> cum_chn_count(feature_blobs.size(), 0);
    for(int i=0; i<feature_blobs.size(); i++){
      total_channels +=  feature_blobs[i]->channels();
    }

    if(num_threads < 1){
        num_threads = ::utils::system::get_available_logical_cpus();
    }

    if(0){ //FIXME
      features.resize(total_channels);
      boost::thread_pool::executor e(num_threads);
      int start_idx = 0;
      for(int i=0; i<feature_blobs.size(); i++){

        if(i > 0){
          start_idx += feature_blobs[i-1]->channels();
        }
        e.submit(boost::bind(&extract_and_resize_mt, image.size(), feature_blobs[i], features, resize_to_img_size, start_idx));
      }
      e.join_all();
    }
    else{
      features.reserve(total_channels);
      for(int i=0; i<feature_blobs.size(); i++){
        int height = feature_blobs[i]->height();
        int width = feature_blobs[i]->width();
        int channels = feature_blobs[i]->channels();
        int dim = width * height;

        for (int c=0; c<channels; c++){
          cv::Mat_<float> feat_mat(cv::Mat(height, width, CV_32FC1, feature_blobs[i]->mutable_cpu_data()+dim*c));
          if(resize_to_img_size){
            resize(feat_mat, feat_mat, cv::Size(image.cols, image.rows), CV_INTER_CUBIC);
          }
          features.push_back(feat_mat);
          if(0){
            imshow("image", image);
            normalize(feat_mat, feat_mat, 1, 0, CV_MINMAX);
            imshow("feat_mat", feat_mat);
            waitKey(0);
          }
        }
      }
    }
    return true;
}

bool CNNFeatures::extract_and_save(Mat image, vector<string> feat_names, vector<cv::Mat_<float> >& features, bool resize_to_img_size, std::string path){

  extract(image, feat_names, features, resize_to_img_size);

    try{
      std::ofstream ofs(path.c_str());
      if(ofs==0){
      LOG(INFO)<<"Error: Cannot open the given path to save features.";
      return false;
      }
      boost::archive::text_oarchive oa(ofs);
      oa<<features;
      ofs.flush();
      ofs.close();
      LOG(INFO)<<"Features saved at :"<<path;
      return true;
    }
    catch(boost::archive::archive_exception& ex){
      LOG(INFO)<<"Archive exception during deserialization:" <<std::endl;
      LOG(INFO)<< ex.what() << std::endl;
      LOG(INFO)<< "it was file: "<<path;
    }
    return true;
}

bool CNNFeatures::load_features(vector<Mat>& features, string path)
{
    std::ifstream ifs(path.c_str());
    if(!ifs){
      LOG(INFO)<<"file not found.";
    }
    else{
      try{
        boost::archive::text_iarchive ia(ifs);
        ia>>features;
        LOG(INFO)<<"Features loaded";
        return true;
      }
      catch(boost::archive::archive_exception& ex){
        LOG(INFO)<<"Reload Tree: Archive exception during deserializiation: "
              <<ex.what();
          LOG(INFO)<<"not able to load features from: "<<path;
      }
    }
    return false;
}

//TODO: Incorporate batch processing!!!!
bool CNNFeatures::extract_all_and_save(const std::vector<cv::Mat> images,
                                        const std::vector<std::string> img_names,
                                        const vector<string> feat_names,
                                        std::string dir_path,
                                        std::string database_type)
{
//    CHECK(images.size());
//
//    int num_feat = feat_names.size();
//
//    std::vector<boost::shared_ptr<caffe::db::DB> > feature_dbs;
//    std::vector<boost::shared_ptr<caffe::db::Transaction> > txns;
//    for (size_t i = 0; i < num_feat; ++i) {
//      boost::filesystem::path dataset_name(dir_path+"/"+feat_names[i]);
//
//      caffe::db::Mode db_mode;
//      if(boost::filesystem::exists(dataset_name)){
//        LOG(INFO)<<"Dataset already exists. Opening existing database in WRITE mode: "<< dataset_name;
//        db_mode = caffe::db::WRITE;
//      }
//      else{
//        db_mode = caffe::db::NEW;
//        LOG(INFO)<< "Opening dataset " << dataset_name;
//      }
//
//      boost::shared_ptr<caffe::db::DB> db(caffe::db::GetDB(database_type.c_str()));
//
//      db->Open(dataset_name.string(), db_mode);
//      feature_dbs.push_back(db);
//      boost::shared_ptr<caffe::db::Transaction> txn(db->NewTransaction());
//      txns.push_back(txn);
//    }
//
//    const int kMaxKeyStrLength = 100;
//    char key_str[kMaxKeyStrLength];
//
//    Datum datum;
//    for(int m=0; m < images.size(); m++){
//      cv::Mat image = images[m];
//
//      std::vector<boost::shared_ptr<Blob<float> > > feature_blobs;
//      get_caffe_blobs(image, feat_names, feature_blobs);
//      CHECK_EQ(feat_names.size(), feature_blobs.size());
//
//      for(int i=0; i<feature_blobs.size(); i++){
//        datum.set_height(feature_blobs[i]->height());
//        datum.set_width(feature_blobs[i]->width());
//        datum.set_channels(feature_blobs[i]->channels());
//        datum.clear_data();
//        datum.clear_float_data();
//
//        const float* feat_blob_data = feature_blobs[i]->cpu_data();
//
//        int dim_features = feature_blobs[i]->count();
//        for (int d = 0; d < dim_features; ++d) {
//          datum.add_float_data(feat_blob_data[d]);
//        }
//
////        int length = snprintf(key_str, kMaxKeyStrLength, img_names[m].c_str());
//        int length = snprintf(key_str, kMaxKeyStrLength, "%010d", m);
//        string out;
//        CHECK(datum.SerializeToString(&out));
//        txns.at(i)->Put(std::string(key_str, length), out);
//
//        if (m % 1000 == 0) {
//          txns.at(i)->Commit();
//          txns.at(i).reset(feature_dbs.at(i)->NewTransaction());
//          LOG(ERROR)<< "Extracted features of " << m <<
//              " query images for feature blob " << feat_names[i];
//        }
//      }
//    }
}

bool CNNFeatures::WrapInputLayer(vector<cv::Mat>& input_channels){
    Blob<float>* input_layer = net->input_blobs()[0];
    int width = input_layer->width();
    int height = input_layer->height();
    float *input_data = input_layer->mutable_cpu_data();
    for(int i=0; i<input_layer->channels(); i++){
      cv::Mat channel(height, width, CV_32FC1, input_data);
      input_channels.push_back(channel);
      input_data += width * height;
    }
    return true;
}

bool CNNFeatures::set_mean_pixel(cv::Scalar mc){
  channel_mean =  mc;
  return true;
}

cv::Scalar CNNFeatures::get_mean_pixel(){
  return channel_mean;
}

cv::Size CNNFeatures::get_input_geometry()
{
  return input_geometry;
}

CNNFeatures::~CNNFeatures()
{
  //dtor
}


} /* namespace features */
} /* namespace vision */
