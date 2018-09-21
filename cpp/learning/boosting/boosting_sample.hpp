/*
 * sample.hpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#ifndef LEARNING_COMMON_SAMPLE_HPP_
#define LEARNING_COMMON_SAMPLE_HPP_

#include <map>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "cpp/learning/common/image_sample.hpp"
#include "cpp/vision/features/feature_channels/feature_channel_factory.hpp"

namespace learning {
namespace boosting {

using vision::features::feature_channels::FeatureChannelFactory;

// Vector of float features.
typedef std::vector<float> LocalPatch;

// Sample type that is implemented using a map between Feature types and
// associated LocalPatches.
template <class Feature>
class BoostingSample : public learning::common::ImageSample {
public:
	BoostingSample(const learning::common::Image* image, const cv::Rect& bbox,
								 bool label)
		: ImageSample(image, bbox),
			label_(label) {}

	bool label() const {
		return label_;
	}

	int int_label() const {
		return label_ ? 1 : -1;
	}

	int patch_count() const {
		return patches_.size();
	}

	// Returns the patch associated with feature.
	const LocalPatch& patch(const Feature& feature) const {
		CHECK(patches_.find(feature) != patches_.end()) << "Can't find patch\n";
		return patches_.find(feature)->second;
	}

	// Extracts patch for a single feature type. The patch is stored in the
	// sample.
	void extract_patch(const Feature& f) {
		patches_[f] = LocalPatch();
		f.extract(get_image()->get_feature_channels(), get_roi(), patches_[f]);
		patches_[f].push_back(1.0f);
	}

	// Extracts patches for all the given features. The patches are stored in
	// the sample.
	void extract_patches(const std::vector<Feature>& features) {
		for (size_t i = 0; i < features.size(); ++i) {
			extract_patch(features[i]);
		}
	}

	// Constant method that either returns the existing patch extracted for
	// feature, or extracts it (mainly used for prediction).
	void generate_patch(const Feature& feature, LocalPatch* patch) const {
		if (patches_.find(feature) != patches_.end()) {
			*patch = patches_.find(feature)->second;
			return;
		}
		feature.extract(get_image()->get_feature_channels(), get_roi(), *patch);
		patch->push_back(1.0f);
	}

	void show() {
		get_image()->display_feature_channels();
	}
	
	virtual ~BoostingSample() {}

private:
	bool label_;

	std::map<Feature, LocalPatch> patches_;
};

} // namespace boosting
} // namespace learning

#endif /* LEARNING_COMMON_SAMPLE_HPP_ */
