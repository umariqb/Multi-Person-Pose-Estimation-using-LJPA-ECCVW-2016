/*
 * weak_classifier.cpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#include "weak_classifier.hpp"

#include "boosting_sample.hpp"
#include "cpp/utils/libsvm/liblinear.hpp"
#include "cpp/vision/features/simple_feature.hpp"

namespace learning {
namespace boosting {

template <class Sample, class Feature>
void WeakClassifier<Sample, Feature>::train(
		const Feature& feature,
		const std::vector<Sample*>& samples,
		const std::string& solver_type_name, double solver_cost) {
	feature_ = feature;

	const size_t num_samples = samples.size();
	const size_t feature_count = samples[0]->patch(feature).size();

	::utils::liblinear::ProblemHolder p;
	p.allocate(num_samples, num_samples * feature_count);

	for (size_t i = 0; i < num_samples; ++i) {
		cv::Mat mat(samples[i]->patch(feature));
		p.push_problem<float>(mat.t(), samples[i]->int_label());
	}

	boost::shared_ptr< ::utils::liblinear::LinearHolder> model;
	model = boost::shared_ptr< ::utils::liblinear::LinearHolder>(
			new ::utils::liblinear::LinearHolder());

	// Parse solver type from string.
	::utils::liblinear::solver_type::T solver_type;
	if (solver_type_name == "L1R_LR") {
		solver_type = ::utils::liblinear::solver_type::L1R_LR;
	} else if (solver_type_name == "L2R_LR") {
		solver_type = ::utils::liblinear::solver_type::L2R_LR;
	} else if (solver_type_name == "L2R_SVC") {
		solver_type = ::utils::liblinear::solver_type::L2R_L2LOSS_SVC_DUAL;
	} else {
		LOG(INFO) << "Unknown solver type " << solver_type_name 
							<< " using L2R_LR.\n";
		solver_type = ::utils::liblinear::solver_type::L2R_LR;
	}

	model->train(p, solver_type, solver_cost);
	model->get_weights(weights_);
}

static double probability(double x) {
		return 1 / (1 + exp(-x));
}

template <class Sample, class Feature>
double WeakClassifier<Sample, Feature>::predict(const Sample& sample) const {
	LocalPatch patch;
	sample.generate_patch(feature_, &patch);

	CHECK(weights_.size() == patch.size())
		<< "Sample has different number of features: "
		<< weights_.size() << " vs " << patch.size()
		<< ".\n";

	double r = 0.0;
	for (size_t i = 0; i < weights_.size(); ++i) {
		r += weights_[i] * patch[i];
	}
	// TODO(andradaq): this is totally not how you should convert from -1,1 to
	// probability, but works reasonable for now.
	return probability(r);
}

template class WeakClassifier<BoostingSample<vision::features::SURFFeature>,
				 											vision::features::SURFFeature>;
template class WeakClassifier<BoostingSample<vision::features::T2SURFFeature>,
				 											vision::features::T2SURFFeature>;


} // namespace boosting
} // namespace learning
