/*
 * test_main.cpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#include <fstream>
#include <vector>

#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "cpp/learning/boosting/adaboost.hpp"
#include "cpp/learning/boosting/boosting_sample.hpp"
#include "cpp/learning/common/eval_utils.hpp"
#include "cpp/learning/common/roc_graphs.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/vision/features/simple_feature.hpp"

using utils::image_file::sample_big_images;
using learning::boosting::AdaBoost;
using learning::boosting::AdaBoostParam;
using learning::boosting::BoostingSample;
using learning::common::Image;
using vision::features::feature_channels::FeatureChannelFactory;
using vision::features::SURFFeature;
using std::vector;

DEFINE_bool(train, true, "Train or load?");
DEFINE_string(classifier_param, "", "General param file");
DEFINE_string(save_file, "", "Classifier file");
DEFINE_string(pos_index_file, "", "Index of test images");
DEFINE_string(neg_index_file, "", "Index of test images");
DEFINE_string(config_file, "", "Config file for AdaBoost");
DEFINE_string(roc_points_file, "/tmp/roc_points.txt",
							"Where to save points for roc curve");
DEFINE_string(test_index,
							"/home/gandrada/sn/tmp/fddb-in/FDDB-fold-01.txt",
							"test index file");

static FeatureChannelFactory fcf;
typedef BoostingSample<SURFFeature> SampleType;

int main(int argc, char** argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);

	CHECK(!FLAGS_save_file.empty()) << "No classifier file.";

	learning::common::ImageClassifierParam classifier_param;
	learning::common::ImageClassifierParam::parse_from_file(
			FLAGS_classifier_param, &classifier_param);

	if (FLAGS_train) {
		CHECK(!FLAGS_pos_index_file.empty()) << "No pos index file provided.";
		CHECK(!FLAGS_neg_index_file.empty()) << "No neg index file provided.";
		CHECK(!FLAGS_config_file.empty()) << "No config file provided.";

		std::vector<cv::Mat> pos_images, neg_images;
		sample_big_images(FLAGS_pos_index_file, 2000, 40, pos_images);
		sample_big_images(FLAGS_neg_index_file, 4000, 40, neg_images);

		LOG(INFO) << "Loaded " << pos_images.size() << " positive images\n";
		LOG(INFO) << "Loaded " << neg_images.size() << " negative images\n";

		vector<const learning::common::Image*> image_ptr;
		vector<SampleType > samples;
		vector<SampleType*> samples_ptr;
		cv::Rect bbox(0, 0, 40, 40);
		samples.reserve(pos_images.size() + neg_images.size());
		for (size_t i = 0; i < pos_images.size(); ++i) {
			samples.push_back(SampleType(
							new Image(pos_images[i], classifier_param.features, fcf),
							bbox, true));
			samples_ptr.push_back(&samples.back());
			image_ptr.push_back(samples.back().get_image());
		}
		for (size_t i = 0; i < neg_images.size(); ++i) {
			samples.push_back(SampleType(
							new Image(neg_images[i], classifier_param.features, fcf),
							bbox, false));
			samples_ptr.push_back(&samples.back());
			image_ptr.push_back(samples.back().get_image());
		}
		LOG(INFO) << "Created samples\n";

		// Load boosting param.
		AdaBoostParam param;
		CHECK(AdaBoostParam::load(FLAGS_config_file, &param));

		// Generate features.
		vector<SURFFeature> train_features(1000);
		for (size_t i = 0; i < train_features.size(); ++i) {
			boost::mt19937 rng(i);
			train_features[i].generate(40, 40, &rng, 4);
		}

		AdaBoost<SampleType, SURFFeature> classifier(param);
		boost::mt19937 gen;
		classifier.train(train_features, samples_ptr, &gen);
		LOG(INFO) << "Training done\n";

		for (size_t i = 0; i < image_ptr.size(); ++i) {
			delete image_ptr[i];
		}

		AdaBoost<SampleType, SURFFeature>::save(FLAGS_save_file, classifier);
	} 
	
	// Test.
	{
		AdaBoost<SampleType, SURFFeature> classifier;
		AdaBoost<SampleType, SURFFeature>::load(FLAGS_save_file, &classifier);

		std::vector<cv::Mat> test_images;
		utils::image_file::load_images(FLAGS_test_index, test_images, 100);

		for (size_t i = 0; i < test_images.size(); ++i) {
			if (test_images[i].rows > 200 || test_images[i].cols > 200) {
				double scale = std::min(200.0 / test_images[i].rows,
																200.0 / test_images[i].cols);
				resize(test_images[i], test_images[i], cv::Size(), scale, scale);
			}
			std::cout << test_images[i].rows << " " << test_images[i].cols << "\n";

			std::vector<learning::common::utils::PeakLocation> detected_faces;
			learning::common::utils::get_peaks<SampleType>(
					test_images[i], classifier, classifier_param, fcf,
					learning::common::utils::PeakDetectionParams(3, 0.5, 0.5, 2, 2),
					&detected_faces, true);
		}
	}

	return 0;
}
