/*
 * cascade.hpp
 *
 *  Created on: Jun 17, 2013
 *      Author: Andrada Georgescu
 */

#ifndef LEARNING_BOOSTING_CASCADE_HPP_
#define LEARNING_BOOSTING_CASCADE_HPP_

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include "cpp/learning/common/classifier_interface.hpp"
#include "cpp/learning/common/image_sample.hpp"
#include "cpp/learning/boosting/adaboost.hpp"
#include "cpp/utils/image_file_utils.hpp"

namespace learning {
namespace boosting {

class CascadeParam {
public:
	CascadeParam() {
		max_pos_count = 0;
	}

	static bool load(const std::string& config_file,
									 CascadeParam *param) {
		std::ifstream ifs(config_file.c_str());
		if (!ifs.good()) {
			return false;
		}
		boost::archive::xml_iarchive ia(ifs);
		ia >> BOOST_SERIALIZATION_NVP(*param);
		return true;
	}

	static bool save(const std::string& config_file,
									 const CascadeParam& param) {
		std::ofstream ofs(config_file.c_str());
		if (!ofs.good()) {
			return false;
		}
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(param);
		return true;
	}

	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_NVP(f_target);
		ar & BOOST_SERIALIZATION_NVP(d_min);
		ar & BOOST_SERIALIZATION_NVP(max_stages);
		ar & BOOST_SERIALIZATION_NVP(adaboost_param);
		ar & BOOST_SERIALIZATION_NVP(max_pos_count);
		ar & BOOST_SERIALIZATION_NVP(pos_index_file);
		ar & BOOST_SERIALIZATION_NVP(neg_index_files);
	}


	// Target FPR.
	double f_target;

	// Minimum hit rate per stage.
	double d_min;

	// Masimum number of stages.
	size_t max_stages;

	// An array of AdaBoost parameters paired with the cascade stage at which
	// they should be used. 
	// This allows to gradually increase the complexity of the boosting
	// detector, increasing the rejection speed for negatives.
	// E.g. <0, boost1>, <3, boost2>: for the first 3 stages (0, 1, 2) boost1
	// will be used as the boosting parameters and then will be changed to
	// boost2.
	std::vector<std::pair<size_t, AdaBoostParam> > adaboost_param;

	// Maximum number of positive samples to use from pos_index_file.
	size_t max_pos_count;

	// Positive samples index file.
	std::string pos_index_file;

	// Index files of images without faces: negative samples will be generated by
	// scanning these images.
	// It's best to use more index files instead of a single one if you want to
	// tune the classifier to learn to distinguish more complex patches at later
	// stages (after it has exausted the basic file).
	std::vector<std::string> neg_index_files;
};

template <class Sample, class Feature>
class CascadeClassifier :
	virtual public learning::common::ImageClassifier<Sample> {
public:
	CascadeClassifier() {}

	CascadeClassifier(const CascadeParam& param) : param_(param) {}

	// Trains the cascade classifier. The negative/positives samples are read
	// from the index files in param_.
	// classifier_param are parameters common for every image classifier (like
	// patch size and required image channels). It is needed for evaluation.
	// Each boosting stage is trained using the candidate_features vector and
	// the corresponding parameters from param_.
	// After each cascading stage, the classifier is saved at save_path.
	virtual void train(
			const learning::common::ImageClassifierParam& classifier_param,
			const std::vector<Feature>& candidate_features, 
			const std::string& save_path,
			boost::mt19937* gen);

	// Probability that sample is positive.
	virtual double predict(const Sample& sample) const;

	static bool save(const std::string& path,
									 const CascadeClassifier<Sample, Feature>& classifier) {
    try {
       std::ofstream ofs(path.c_str());
       CHECK(ofs);
       boost::archive::binary_oarchive oa(ofs);
       oa << classifier;
       ofs.flush();
       ofs.close();
       LOG(INFO) << "saved " << path << std::endl;
       return true;
     } catch (boost::archive::archive_exception& ex) {
       LOG(INFO) << "Archive Exception during serializing:" << std::endl;
       LOG(INFO) << ex.what() << std::endl;
       LOG(INFO) << "it was classifier: " << path << std::endl;
     }
     return true;
	}

	static bool load(const std::string& path,
									 CascadeClassifier<Sample, Feature>* classifier) {
		std::ifstream ifs(path.c_str());
		if (!ifs) {
			LOG(INFO) << "Classifier file not found " << path;
		} else {
			try {
				boost::archive::binary_iarchive ia(ifs);
				ia >> *classifier;
				return true;
			} catch (boost::archive::archive_exception& ex) {
				LOG(INFO)
					<< "Reload Classifier: Archive Exception during deserializing: "
					<< ex.what() << std::endl;
				LOG(INFO) << "not able to load  " << path << std::endl;
			}
    }
		return false;
	}

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & this->image_classifier_param_.patch_size;
		ar & this->image_classifier_param_.features;
		ar & stages_;
		ar & thresholds_;
		ar & f_current_;
		ar & d_current_;
	}

	bool predict_bool(const Sample& sample) const {
		for (size_t i = 0; i < stages_.size(); ++i) {
			double r = stages_[i].predict(sample);
			if (r < thresholds_[i]) {
				return false;
			}
		}
		return true;
	}

	void resample_negatives(
			utils::image_file::ImageIterator& image_iterator,
			size_t count, const cv::Rect& bbox,
			const learning::common::ImageClassifierParam& param,
			const vision::features::feature_channels::FeatureChannelFactory& fcf,
			std::vector<learning::common::Image>* neg_images,
			std::vector<Sample>* neg_samples) const;

	std::vector<AdaBoost<Sample, Feature> > stages_;
	std::vector<double> thresholds_;

	CascadeParam param_;

	double f_current_;
	double d_current_;
};

} // namespace boosting
} // namespace learning

#endif /* LEARNING_BOOSTING_CASCADE_HPP_ */
