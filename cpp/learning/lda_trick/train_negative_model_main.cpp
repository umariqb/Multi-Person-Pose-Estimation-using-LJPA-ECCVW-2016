/*
 * train_negative_model_main.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: lbossard
 */

#include <google/gflags.h>
#include <glog/logging.h>

#include <opencv2/core/core.hpp>

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include "cpp/utils/system_utils.hpp"
#include "cpp/utils/file_utils.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/serialization/serialization.hpp"

#include "negative_model.hpp"

DEFINE_string(blocks, "", "path to dire of *.mat with serialized cv mats. each row is one feature. or txtfile with list of cv mat files");
DEFINE_string(model_output, "", "outputfile");
DEFINE_int32(num_threads, 0, "max number of threads. if set to 0, will be determined automatically");
DEFINE_double(covar_regualizer, 0, "regularizer added to the covariance matrix bevore inverting");

int main(int argc, char** argv){
  google::InstallFailureSignalHandler();
  google::LogToStderr();
  google::InitGoogleLogging(argv[0]);
  // get command line args
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (false){
    google::ShowUsageWithFlags(argv[0]);
    return -1;
  }

  const fs::path blocks = FLAGS_blocks;
  const fs::path model_output = FLAGS_model_output;
  const double covar_regularizer = FLAGS_covar_regualizer;

  // set threads
  //TODO: this probably wont work
  int num_threads = FLAGS_num_threads;
  if (num_threads < 1){
    num_threads = utils::system::get_available_logical_cpus();
  }
  cv::setNumThreads(num_threads);
  LOG(INFO) << "using " << num_threads << " threads";



  //----------------------------------------------------------------------------
  // collect files
  std::vector<fs::path> feature_files;
  LOG(INFO) << "collecting feature files from " << blocks;
  if (fs::is_directory(blocks)){
    utils::fs::collect_files(blocks.string(), ".*\\.(mat|cvmat)", std::back_inserter(feature_files));
  }
  else {
    CHECK(utils::image_file::load_paths(blocks.string(), feature_files));
  }
  CHECK_GT(feature_files.size(), 0) << "found no feature files";
  LOG(INFO) << "done" << blocks;

  //----------------------------------------------------------------------------
  // load features
  CHECK(false) << "implement with online covar";

  //----------------------------------------------------------------------------
  // compute covariance
  cv::Mat_<double> covar;
  cv::Mat_<double> mean;

  //----------------------------------------------------------------------------
  // invert cov matrix

  // regularize covariance matrix
  if (covar_regularizer > 0){
    LOG(INFO) << "adding regularizer ";
    CHECK_EQ(covar.rows, covar.cols);
    for (int i = 0; i < covar.rows; ++i){
      covar(i, i) += covar_regularizer;
    }
    LOG(INFO) << "adding regularizer... done";
  }

  cv::Mat_<double> covar_inverted;
  CHECK(0 < cv::invert(covar, covar_inverted, cv::DECOMP_LU)) << "covariance is singular";

  //----------------------------------------------------------------------------
  // save
  awesomeness::learning::lda_trick::NegativeModel neg_model;
  mean.copyTo(neg_model.mutable_mu_neg());
  covar_inverted.copyTo(neg_model.mutable_sigma_inv());

  utils::serialization::write_binary_archive(model_output.string(), neg_model);
  LOG(INFO) << "Finished";

}

