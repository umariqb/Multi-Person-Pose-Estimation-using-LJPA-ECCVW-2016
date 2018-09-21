/*
 * cascade_test.cpp
 *
 *  Created on: Aug 23, 2013
 *      Author: Andrada Georgescu
 *
 * Test trained classifier on sample.
 * cascade_test --save_file=sample_cascade_classifier.save
 * 							--classifier_param=test_data/image_clasifier_params.txt
 *							--test_index=/srv/glusterfs/gandrada/training_data/fddb-faces/index.txt
 */

#include <vector>

#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "cpp/learning/boosting/boosting_sample.hpp"
#include "cpp/learning/boosting/cascade.hpp"
#include "cpp/learning/common/eval_utils.hpp"
#include "cpp/learning/common/roc_graphs.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/vision/features/simple_feature.hpp"

using utils::image_file::sample_big_images;
using learning::boosting::BoostingSample;
using learning::boosting::CascadeClassifier;
using learning::boosting::CascadeParam;
using learning::common::Image;
using vision::features::feature_channels::FeatureChannelFactory;
using vision::features::SURFFeature;
using vision::features::T2SURFFeature;
using std::vector;

DEFINE_string(save_file, "", "Classifier file");
DEFINE_string(test_index, "", "test index file");
DEFINE_int32(count, 1000, "How many to test");
DEFINE_string(stat_file, "", "Where to store classifier results");

static FeatureChannelFactory fcf;
typedef T2SURFFeature FeatureType;
typedef BoostingSample<FeatureType> SampleType;

int main(int argc, char** argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);

	CHECK(!FLAGS_save_file.empty()) << "No classifier file.";
	CHECK(!FLAGS_stat_file.empty()) << "No results file.";

	CHECK(!FLAGS_test_index.empty());
	CascadeClassifier<SampleType, FeatureType> classifier;
	CascadeClassifier<SampleType, FeatureType>::load(FLAGS_save_file,
																									 &classifier);
	learning::common::ImageClassifierParam classifier_param =
		classifier.image_classifier_param();

	std::vector<boost::filesystem::path> paths;
	utils::image_file::load_paths(FLAGS_test_index, paths);
	std::vector<cv::Mat> test_images;
	utils::image_file::load_images(paths, test_images, FLAGS_count);

	cv::Rect bbox(0, 0, classifier_param.patch_size.width,
								classifier_param.patch_size.height);
	std::ofstream results;
	results.open(FLAGS_stat_file.c_str());
	for (size_t i = 0; i < test_images.size(); ++i) {
		Image image(test_images[i], classifier_param.features, fcf, true);
		const double p = classifier.predict(SampleType(&image, bbox, true)); 
		results << p << " ";
	}
	results.close();

	return 0;
}
