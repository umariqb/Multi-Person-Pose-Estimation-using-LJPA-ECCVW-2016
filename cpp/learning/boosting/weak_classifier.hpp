/*
 * weak_classifier.hpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#ifndef LEARNING_BOOSTING_WEAK_CLASSIFIER_HPP_
#define LEARNING_BOOSTING_WEAK_CLASSIFIER_HPP_

#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "cpp/learning/common/classifier_interface.hpp"
#include "cpp/vision/features/simple_feature.hpp"


namespace learning {
namespace boosting {

template <class Sample, class Feature>
class WeakClassifier
	: virtual public learning::common::ClassifierInterface<Sample> {
public:
	WeakClassifier() {}

	Feature feature() {
		return feature_;
	}

	// Trains the classifier on the training_set, using the feature vectors
	// associated with feature.
	virtual void train(const Feature& feature,
										 const std::vector<Sample*>& training_set,
										 const std::string& solver_type = "",
										 double solver_cost = 1.0);

	// Probability that sample is positive.
	virtual double predict(const Sample& sample) const;

	virtual ~WeakClassifier() {}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & feature_;
		ar & weights_;
	}

	Feature feature_;

	std::vector<double> weights_;
};

} // namespace boosting
} // namespace learning

#endif /* LEARNING_BOOSTING_WEAK_CLASSIFIER_HPP_ */
