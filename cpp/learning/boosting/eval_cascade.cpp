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

#include "cpp/learning/boosting/boosting_sample.hpp"
#include "cpp/learning/boosting/cascade.hpp"
#include "cpp/learning/common/eval_utils.hpp"
#include "cpp/learning/common/roc_graphs.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/vision/features/simple_feature.hpp"

using utils::image_file::load_images;
using utils::image_file::load_paths;
using learning::boosting::BoostingSample;
using learning::boosting::CascadeClassifier;
using learning::boosting::CascadeParam;
using learning::common::Image;
using learning::common::utils::PeakDetectionParams;
using learning::common::utils::PeakLocation;
using vision::features::feature_channels::FeatureChannelFactory;
using vision::features::SURFFeature;
using vision::features::T2SURFFeature;

using cv::Mat;
using namespace std;

DEFINE_string(peak_params, "", "peak_detection_param");
DEFINE_string(save_file, "", "Classifier file");
DEFINE_string(index_file, "", "Index of test images");
DEFINE_string(output_file, "", "Index of test images");

static FeatureChannelFactory fcf;
typedef T2SURFFeature FeatureType;
typedef BoostingSample<FeatureType> SampleType;

void read_features(const cv::Size& sample_size,
									 vector<FeatureType>& features) {
	std::ifstream in("/home/gandrada/sn/tmp/surf_features.txt");
	int n;
	in >> n;

	for (int i = 0; i < n; ++i) {
		int x, y, w, h, xr, yr;
		in >> x >> y >> w >> h >> xr >> yr;
		features.push_back(FeatureType(sample_size.width, sample_size.height,
																	 cv::Rect(x, y, w, h), xr, yr));
	}
}

void generate_features(const cv::Size& sample_size,
											 vector<FeatureType>& features, int count) {
	features.resize(count);
	for (size_t i = 0; i < features.size(); ++i) {
		boost::mt19937 rng(i);
		features[i].generate(sample_size.width, sample_size.height,
												 &rng, 4);
	}
}

static bool load_image_samples(const string& index_file,
															 const vector<int>& features, 
															 vector<boost::filesystem::path>& test_img_paths,
															 vector<Mat>* orig_images,
															 vector<Image>* image_samples,
															 vector<double>& scales) {
	// Load index.
	assert(load_paths(index_file, test_img_paths));

	// Load image content.
	load_images(index_file, *orig_images, test_img_paths.size(), 1);

	vector<Mat>& test_images = *orig_images;
	scales.resize(test_images.size());
	for (size_t i = 0; i < test_images.size(); ++i) {
			if (test_images[i].rows > 200 || test_images[i].cols > 200) {
				double scale = std::min(200.0 / test_images[i].rows,
																200.0 / test_images[i].cols);
				scales[i] = scale;
				resize(test_images[i], test_images[i], cv::Size(), scale, scale);
			} else {
				scales[i] = 1.0;
			}
	}


	// Generate features.
	FeatureChannelFactory fcf;
	image_samples->reserve(orig_images->size());
	for(size_t i = 0; i < orig_images->size(); ++i) {
		image_samples->push_back(Image((*orig_images)[i], features, fcf, true, i));
	}

	return true;
}

int do_scale(double s, int x) {
	return static_cast<int>(static_cast<double>(x) / s);
}


int main(int argc, char** argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);

	CHECK(!FLAGS_save_file.empty()) << "No classifier file.";

	learning::common::utils::PeakDetectionParams pdp(7, 0.9, 0.9, 2, 1);
	if (!FLAGS_peak_params.empty()) {
		pdp.parse_from_file(FLAGS_peak_params);
	}
	CascadeClassifier<SampleType, FeatureType> classifier;
	CascadeClassifier<SampleType, FeatureType>::load(FLAGS_save_file, &classifier);
	
	learning::common::ImageClassifierParam classifier_param =
		classifier.image_classifier_param();

	vector<boost::filesystem::path> test_img_paths;
	vector<Mat> orig_images;
	vector<Image> img_samples;
	vector<double> scales;
	load_image_samples(FLAGS_index_file, classifier_param.features,
										 test_img_paths, &orig_images,
										 &img_samples, scales);

	ofstream results;
	results.open(FLAGS_output_file.c_str());
	for (size_t i = 0; i < img_samples.size(); ++i) {
		LOG(INFO) << "Getting foreground map for image sample " << i << "\n";

		vector<PeakLocation> detected_faces;
		learning::common::utils::get_peaks(orig_images[i], classifier, classifier_param,
																			 fcf, pdp, &detected_faces, false);

		// Remove root path and extension for fddb evaluation.
		string image_id = test_img_paths[i].string();
		image_id.replace(0, strlen("/srv/glusterfs/gandrada/fddb/originalPics/"),
										 "");
		image_id.replace(image_id.length() - strlen(".jpg"), strlen(".jpg"), "");

		results << image_id << "\n";
		results << detected_faces.size() << "\n";
		LOG(INFO) << "Detected " << detected_faces.size() << " faces\n";
		for (int j = 0; j < detected_faces.size(); ++j) {
			results << do_scale(scales[i], detected_faces[j].x - detected_faces[j].size.width / 2) << " "
				      << do_scale(scales[i], detected_faces[j].y - detected_faces[j].size.height / 2) << " "
				      << do_scale(scales[i], detected_faces[j].size.width) << " " 
							<< do_scale(scales[i], detected_faces[j].size.height) << " "
					    << detected_faces[j].score << "\n";
		}
	}
	results.close();

	return 0;
}
