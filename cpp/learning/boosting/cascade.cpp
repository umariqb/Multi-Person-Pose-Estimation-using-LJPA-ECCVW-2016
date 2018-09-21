/*
 * cascade.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: Andrada Georgescu
 */

#include "cpp/learning/boosting/cascade.hpp"

#include <string>
#include <utility>
#include "cpp/learning/boosting/boosting_sample.hpp"
#include "cpp/learning/common/image_sample.hpp"
#include "cpp/learning/common/roc_graphs.hpp"
#include "cpp/utils/image_file_utils.hpp"
#include "cpp/vision/features/simple_feature.hpp"

namespace learning {
namespace boosting {

using std::pair;
using std::string;
using std::vector;
using learning::common::Image;
using learning::common::find_roc_point;
using utils::image_file::sample_big_images;
using vision::features::feature_channels::FeatureChannelFactory;
using vision::features::SURFFeature;

template <class Sample, class Feature>
void CascadeClassifier<Sample, Feature>::train(
		const learning::common::ImageClassifierParam& classifier_param,
		const std::vector<Feature>& train_features, 
		const std::string& save_path,
		boost::mt19937* gen) {
	LOG(INFO) << "Cascade::train\n";

	this->set_image_classifier_param(classifier_param);

	// Initialize if loading failed.
	if (!CascadeClassifier::load(save_path, this)) {
		stages_.clear();
		thresholds_.clear();
		f_current_ = 1.0;
		d_current_ = 1.0;
	}
	
	if (stages_.size() >= param_.max_stages) {
		LOG(INFO) << "Classifier already fully trained.\n";
		return;
	}

	// Create FCF to use for all samples.
	FeatureChannelFactory fcf;

	// Load all positive images.
	const cv::Size& sample_size = classifier_param.patch_size;
	std::vector<cv::Mat> pos_images;
	sample_big_images(param_.pos_index_file, param_.max_pos_count,
										sample_size.width, pos_images);
	const size_t pos_count = pos_images.size();
	LOG(INFO) << "Loaded " << pos_count << " positive samples.\n";

	vector<Sample> pos_samples;
	vector<Image> pos_mem(pos_count);
	pos_samples.reserve(pos_count);
	cv::Rect bbox(0, 0, sample_size.width, sample_size.height);
	for (size_t i = 0; i < pos_images.size(); ++i) {
		pos_mem[i] = Image(pos_images[i], classifier_param.features, fcf,
											 true);
		pos_samples.push_back(Sample(&pos_mem[i],	bbox, true));
	}

	// Find Adaboost param for first stage.
	size_t param_index = 0;
	while (param_index < param_.adaboost_param.size() - 1 &&
				 stages_.size() >= param_.adaboost_param[param_index + 1].first) {
		++param_index;
	}
	LOG(INFO) << "Starting from stage " << stages_.size()
						<< " with param[" << param_index << "] : "
						<< param_.adaboost_param[param_index].first << "\n";

	// Negative samples generator.
	utils::image_file::ImagePatchIterator image_iterator(
			param_.neg_index_files, classifier_param.patch_size, 3, 0.5, 10);

	vector<Image> neg_mem(pos_count);
	while (true) {
		LOG(INFO) << "Cascading round " << stages_.size() << "\n";
	
		vector<Sample> neg_samples;
		resample_negatives(image_iterator, pos_count, bbox, classifier_param, fcf,
											 &neg_mem, &neg_samples);
	
		std::vector<Sample*> samples_ptr;
		for (size_t i = 0; i < pos_samples.size(); ++i) {
			samples_ptr.push_back(&pos_samples[i]);
		}
		for (size_t i = 0; i < neg_samples.size(); ++i) {
			samples_ptr.push_back(&neg_samples[i]);
		}

		// Add new boosting stage.
		LOG(INFO) << "Create new AdaBoost.\n";
		stages_.push_back(AdaBoost<Sample, Feature>(
						param_.adaboost_param[param_index].second));
		AdaBoost<Sample, Feature>& classifier = stages_.back();
		classifier.train(train_features, samples_ptr, gen);

		// Evaluate on the whole training set to obtain ROC curve.
		LOG(INFO) << "Evaluating new stage.\n";
		{
			vector<std::pair<double, bool> > scores;
			scores.reserve(samples_ptr.size());
			for (size_t i = 0; i < samples_ptr.size(); ++i) {
				scores.push_back(std::make_pair(classifier.predict(*samples_ptr[i]),
																				samples_ptr[i]->label()));
			}
			double fpoint, dpoint;
			const double th = find_roc_point(scores, param_.d_min,
																			 &fpoint, &dpoint);
			LOG(INFO) << "ROC point: "
								<< fpoint << " " << dpoint << " : " << th << "\n";
			f_current_ *= fpoint;
			LOG(INFO) << "FPR: " << f_current_ << "\n";
			d_current_ *= dpoint;
			thresholds_.push_back(th);

			vector<std::pair<double, double> > unused;
			std::ostringstream oss;
			oss << "/home/gandrada/sn/tmp/aug26/roc_" << (stages_.size() - 1) << ".txt";
			classifier.print_roc_points(samples_ptr, oss.str(), unused);
		}
		
		CascadeClassifier::save(save_path, *this);

		LOG(INFO) << "Testing target FPR.\n";
		if (f_current_ <= param_.f_target) {
			LOG(INFO) << "Reached target FPR... stopping.\n";
			break;
		}

		if (stages_.size() == param_.max_stages) {
			LOG(INFO) << "Stopping at " << param_.max_stages << " stages\n";
			break;
		}

		// Modify AdaBoost param after a few stages.
		if (param_index < param_.adaboost_param.size() - 1 &&
				stages_.size() == param_.adaboost_param[param_index + 1].first) {
			++param_index;
		}
	}

	// TODO(andradaq): Add an extra stage that tries to discriminate between
	// faces and almost-faces (hands, skin-patches).
}

template <class Sample, class Feature>
double CascadeClassifier<Sample, Feature>::predict(
		const Sample& sample) const {
	for (size_t i = 0; i < stages_.size(); ++i) {
		double r = stages_[i].predict(sample);
		if (r < thresholds_[i] || i == stages_.size() - 1) {
			return (i + r) / stages_.size();
		}
	}
	return 1;
}

template <class Sample, class Feature>
void CascadeClassifier<Sample, Feature>::resample_negatives(
		utils::image_file::ImageIterator& image_iterator,
		size_t count, const cv::Rect& bbox,
		const learning::common::ImageClassifierParam& classifier_param, 
		const vision::features::feature_channels::FeatureChannelFactory& fcf,
		std::vector<Image>* neg_mem,
		std::vector<Sample>* neg_samples) const {
	// Empty negative set and add new.
	LOG(INFO) << "Resample negatives.\n";
	neg_mem->resize(count);

	size_t tested_samples = 0;
	while (neg_samples->size() < count) {
		if (!image_iterator.has_next()) {
			LOG(INFO) << "No more negative samples!\n";
			break;
		}
		cv::Mat next_image = image_iterator.next();
		Image image(next_image, classifier_param.features, fcf, true);
		Sample s(&image, bbox, false);
		++tested_samples;
		if (predict_bool(s)) {
			(*neg_mem)[neg_samples->size()] = image;
			neg_samples->push_back(Sample(&(*neg_mem)[neg_samples->size()], bbox,
																		false));
			if (neg_samples->size() % 1000 == 0) {
				LOG(INFO) << "Tested " << tested_samples << ": " << neg_samples->size() 
					<< " out of " << count << "\n";
			}
		}
		if (neg_samples->size() == count) {
			break;
		}
	}
	LOG(INFO) << "Negative samples tested : " << tested_samples << "\n";
}

template class CascadeClassifier<BoostingSample<vision::features::SURFFeature>,
				                         vision::features::SURFFeature>;
template class CascadeClassifier<BoostingSample<vision::features::T2SURFFeature>,
				                         vision::features::T2SURFFeature>;

} // namespace boosting
} // namespace learning
