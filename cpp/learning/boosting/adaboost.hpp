/*
 * adaboost.hpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#ifndef LEARNING_BOOSTING_ADABOOST_HPP_
#define LEARNING_BOOSTING_ADABOOST_HPP_

#include <fstream>
#include <string>
#include <vector>

#include <boost/random.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <glog/logging.h>

#include "cpp/learning/boosting/weak_classifier.hpp"
#include "cpp/learning/common/classifier_interface.hpp"
#include "cpp/utils/system_utils.hpp"

namespace learning {
namespace boosting {

struct AdaBoostParam {
	AdaBoostParam() : rounds(0) {}

	AdaBoostParam(size_t r, size_t s)
		: rounds(r),
			sampled_count(s) {}

	static bool load(const std::string& config_file,
									 AdaBoostParam *param) {
		std::ifstream ifs(config_file.c_str());
		if (!ifs.good()) {
			return false;
		}
		boost::archive::xml_iarchive ia(ifs);
		ia >> BOOST_SERIALIZATION_NVP(*param);
		return true;
	}

	static bool save(const std::string& config_file,
									 const AdaBoostParam& param) {
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
		ar & BOOST_SERIALIZATION_NVP(rounds);
		ar & BOOST_SERIALIZATION_NVP(sampled_count);
		ar & BOOST_SERIALIZATION_NVP(solver_type);
		ar & BOOST_SERIALIZATION_NVP(solver_cost); 
	}

	// The maximum number of boosting rounds allowed.
	size_t rounds;

	// Number of positive and negative samples to use in each round from the
	// whole set.
	size_t sampled_count;

	// Weak classifier algorithm.
	std::string solver_type;

	// Regularization parameter.
	double solver_cost;
};

template <class Sample, class Feature>
class AdaBoost : virtual public learning::common::ClassifierInterface<Sample> {
public:
	AdaBoost() : exclude_classifier_(-1) {}

	AdaBoost(const AdaBoostParam& param)
		: param_(param),
			exclude_classifier_(-1) {}

	// Trains AdaBoost classifier using the candidate_features vector on the
	// training_set.
	void train(
			const std::vector<Feature>& candidate_features, 
			const std::vector<Sample*>& training_set,
			boost::mt19937* gen);

	// Probability that a sample is positive.
	virtual double predict(const Sample& sample) const;

	// Prints ROC curve points for a set of testing samples.
	void print_roc_points(
			const std::vector<Sample*>& samples,
			const std::string& filename,
			std::vector<std::pair<double, double> >& roc_points) const;

	static bool save(const std::string& path,
									 const AdaBoost<Sample, Feature>& classifier) {
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
									 AdaBoost<Sample, Feature>* classifier) {
		std::ifstream ifs(path.c_str());
		if (!ifs) {
			LOG(INFO) << "tree not found " << path;
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

	~AdaBoost() {}

private:
	void update_weights(
			const std::vector<Sample*>&	samples,
			std::vector<double>& weights);

	void train_candidate(
			const Feature& feature, const std::vector<Sample*>& sampled,
			const std::vector<Sample*>& all_samples,
			WeakClassifier<Sample, Feature>* candidate, double* score) const {
		candidate->train(feature, sampled, param_.solver_type, param_.solver_cost);
		*score = evaluate_candidate(all_samples, *candidate);
	}

	double best_classifier(
			const std::vector<Feature>& candidate_features,
			const std::vector<Sample*>& sampled,
			const std::vector<Sample*>& training_set,
			WeakClassifier<Sample, Feature>* best) const;

	void backward_removing(const std::vector<Sample*>& training_set);

	double evaluate(const std::vector<Sample*>& samples) const;

	double evaluate_candidate(
			const std::vector<Sample*>& samples,
			const WeakClassifier<Sample, Feature>& candidate) const;

	double predict_next(const WeakClassifier<Sample, Feature>& next,
											const Sample& sample) const {
		return (predict(sample) * classifiers_.size() + next.predict(sample)) /
			(classifiers_.size() + 1);
	}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & classifiers_;
	}

	AdaBoostParam param_;

	std::vector<WeakClassifier<Sample, Feature> > classifiers_;

	int exclude_classifier_;
};



} // namespace boosting
} // namespace learning

#endif /* LEARNING_BOOSTING_ADABOOST_HPP_ */
