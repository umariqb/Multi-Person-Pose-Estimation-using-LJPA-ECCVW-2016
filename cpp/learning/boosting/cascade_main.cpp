/*
 * cascade_main.cpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 *
 * Runs the CascadeClassifier in FLAGS_save_file on the image in
 * FLAGS_test_index, optionally training it before. 
 *
 * Sample training step:
 * cascade_main --train=true
 *							--save_file=sample_cascade_classifier.save
 *							--config_file=test_data/sample_cascade_config.txt
 *							--features_file=test_data/t2surf_features.txt
 * 							--classifier_param=test_data/image_clasifier_params.txt
 *							--test_index=/srv/glusterfs/gandrada/FDDB-index-files/FDDB-fold-01.txt
 *							--peak_params=test_data/peak_detection_params.txt
 *
 * To only test an already trained classifier set --train=false.
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

DEFINE_bool(train, true, "Train or load?");
DEFINE_string(save_file, "", "Classifier file");
DEFINE_string(test_index, "", "test index file");
DEFINE_string(peak_params, "", "Peak Detection param file");

// Training step only:
DEFINE_string(classifier_param, "", "Image classifier param file");
DEFINE_string(config_file, "", "Config file for Cascade");
DEFINE_string(features_file, "", "File listing the T2 SURF features to be used"
							"for training. If not specified, 1000 of them are randomly"
							"generated.");

static FeatureChannelFactory fcf;
typedef T2SURFFeature FeatureType;
typedef BoostingSample<FeatureType> SampleType;

// Randomly generates count features of feature type.
// Only SURF and T2SURF supported for now.
void generate_features(const cv::Size& sample_size,
											 vector<FeatureType>& features, int count) {
	features.resize(count);
	for (size_t i = 0; i < features.size(); ++i) {
		boost::mt19937 rng(i);
		features[i].generate(sample_size.width, sample_size.height,
												 &rng, 4);
	}
}

void read_features(const cv::Size& sample_size,
									 vector<FeatureType>& features) {
	if (FLAGS_features_file.empty()) {
		generate_features(sample_size, features, 1000);
		return;
	}

	std::ifstream in(FLAGS_features_file.c_str());
	int n;
	in >> n;

	for (int i = 0; i < n; ++i) {
		int x, y, w, h, xr, yr;
		in >> x >> y >> w >> h >> xr >> yr;
		features.push_back(FeatureType(sample_size.width, sample_size.height,
																	 cv::Rect(x, y, w, h), xr, yr));
	}
}

int main(int argc, char** argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);

	CHECK(!FLAGS_save_file.empty()) << "No classifier file.";

	if (FLAGS_train) {
		learning::common::ImageClassifierParam classifier_param;
		learning::common::ImageClassifierParam::parse_from_file(
				FLAGS_classifier_param, &classifier_param);

		CHECK(!FLAGS_config_file.empty()) << "No config file provided.";
		CascadeParam param;
		CHECK(CascadeParam::load(FLAGS_config_file, &param));
		CascadeClassifier<SampleType, FeatureType> classifier(param);
		boost::mt19937 gen;
		vector<FeatureType> train_features;
		read_features(classifier_param.patch_size, train_features);
		classifier.train(classifier_param, train_features, FLAGS_save_file, &gen);
	}

	// Test.
	if (!FLAGS_train) {
		CHECK(!FLAGS_test_index.empty());
		learning::common::utils::PeakDetectionParams pdp(7, 0.9, 0.9, 2, 1);
		if (!FLAGS_peak_params.empty()) {
			pdp.parse_from_file(FLAGS_peak_params);
		}
		CascadeClassifier<SampleType, FeatureType> classifier;
		CascadeClassifier<SampleType, FeatureType>::load(FLAGS_save_file,
																										 &classifier);
		learning::common::ImageClassifierParam classifier_param =
			classifier.image_classifier_param();

		std::vector<boost::filesystem::path> paths;
		utils::image_file::load_paths(FLAGS_test_index, paths);
		std::vector<cv::Mat> test_images;
		utils::image_file::load_images(paths, test_images, 100);

		for (size_t i = 0; i < test_images.size(); ++i) {
			if (test_images[i].rows > 400 || test_images[i].cols > 400) {
				double scale = std::min(400.0 / test_images[i].rows,
																400.0 / test_images[i].cols);
				resize(test_images[i], test_images[i], cv::Size(), scale, scale);
			}
			std::cout << test_images[i].rows << " " << test_images[i].cols << "\n";

			std::vector<learning::common::utils::PeakLocation> detected_faces;
			learning::common::utils::get_peaks<SampleType>(
					test_images[i], classifier, classifier_param, fcf, pdp,
					&detected_faces, false);

			//learning::common::utils::merge_peaks(&detected_faces, 0.8);
			for (size_t j = 0; j < detected_faces.size(); ++j) {
				cv::Rect r(detected_faces[j].x - detected_faces[j].size.width / 2,
									 detected_faces[j].y - detected_faces[j].size.height / 2,
									 detected_faces[j].size.width,
									 detected_faces[j].size.height);
				cv::rectangle(test_images[i], r,
											// Create some colored scaled to the peak score (highest
											// score will be bright red, going to white).
											cv::Scalar(static_cast<int>(60 * j),
																 static_cast<int>(60 * j),
																 static_cast<int>(255), 0), 3);
			}
			cv::imshow("a", test_images[i]);
			cv::waitKey(0);
		}
	}

	return 0;
}
