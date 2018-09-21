/*
 * adaboost.cpp
 *
 *  Created on: May 15, 2013
 *      Author: Andrada Georgescu
 */

#include "adaboost.hpp"

#include <fstream>

#include <boost/bind.hpp>
#include "cpp/learning/common/sampling.hpp"
#include "cpp/learning/common/roc_graphs.hpp"
#include "cpp/learning/boosting/boosting_sample.hpp"
#include "cpp/utils/system_utils.hpp"
#include "cpp/utils/thread_pool.hpp"
#include "cpp/vision/features/simple_feature.hpp"

namespace learning {
namespace boosting {

using std::vector;
using learning::common::area_under_curve;
using learning::common::utils::sample_subset;
using learning::common::utils::split_samples;

template <class Sample, class Feature>
void AdaBoost<Sample, Feature>::update_weights(
		const std::vector<Sample*>&	samples,
		std::vector<double>& weights) {
	double sum = 0.0;
	for (size_t i = 0; i < samples.size(); ++i) {
		weights[i] *= exp(-samples[i]->int_label() * predict(*samples[i]));
		sum += weights[i];
	}
	const double inv_sum = 1 / sum;
	for (size_t i = 0; i < samples.size(); ++i) {
		weights[i] *= inv_sum;
	}
}

template <class Sample, class Feature>
double AdaBoost<Sample, Feature>::evaluate(const vector<Sample*>& samples) const {
	vector<std::pair<double, bool> > scores;
	scores.reserve(samples.size());
	for (size_t i = 0; i < samples.size(); ++i) {
		scores.push_back(std::make_pair(predict(*samples[i]),
																		samples[i]->label()));
	}
	return area_under_curve(scores);
}

template <class Sample, class Feature>
double AdaBoost<Sample, Feature>::evaluate_candidate(
		const vector<Sample*>& samples,
		const WeakClassifier<Sample, Feature>& classifier) const {
	// For each patch, combine with existing model and evaluate on whole
	// training set to obtain AUC score.
	vector<std::pair<double, bool> > scores;
	scores.reserve(samples.size());
	for (size_t i = 0; i < samples.size(); ++i) {
		scores.push_back(std::make_pair(predict_next(classifier, *samples[i]),
																		samples[i]->label()));
	}
	return area_under_curve(scores);
}

template <class Sample, class Feature>
void AdaBoost<Sample, Feature>::print_roc_points(
		const vector<Sample*>& samples,
		const std::string& filename,
		vector<std::pair<double, double> >& roc_points) const {
	vector<std::pair<double, bool> > scores;
	scores.reserve(samples.size());
	for (size_t i = 0; i < samples.size(); ++i) {
		scores.push_back(std::make_pair(predict(*samples[i]),
																		samples[i]->label()));
	}
	roc_points.clear();
	area_under_curve(scores, &roc_points);
	std::ofstream out(filename.c_str());
	CHECK(out.is_open());
	for (size_t i = 0; i < roc_points.size(); ++i) {
		out << roc_points[i].first << " " << roc_points[i].second << "\n";
	}
	out.close();
}

template <class Sample, class Feature>
double AdaBoost<Sample, Feature>::best_classifier(
		const std::vector<Feature>& candidate_features,
		const std::vector<Sample*>& sampled,
		const std::vector<Sample*>& training_set,
		WeakClassifier<Sample, Feature>* best) const {
	const int n = candidate_features.size();
	LOG(INFO) << "Training weak classifier for each patch.\n";
	vector<WeakClassifier<Sample, Feature> > candidates(n);
	vector<double> scores(n);

	const int num_threads = ::utils::system::get_available_logical_cpus();
	boost::thread_pool::executor e(num_threads);
	for (size_t k = 0; k < n; ++k) {
		// Train logistic regression model for each patch.
		e.submit(boost::bind(&AdaBoost<Sample, Feature>::train_candidate,
												 boost::ref(this),
												 candidate_features[k], sampled, training_set,
												 &candidates[k], &scores[k]));
	}
	e.join_all();

	const int best_index = std::max_element(scores.begin(), scores.end())
		- scores.begin();
	LOG(INFO) << "Best candidate: " << best_index 
						<< " with score: " << scores[best_index] << "\n";

	*best = candidates[best_index];
	return scores[best_index];
}

template <class Sample, class Feature>
void AdaBoost<Sample, Feature>::train(const vector<Feature>& candidate_features,
																			const vector<Sample*>& training_set,
																			boost::mt19937* gen) {
	LOG(INFO) << "AdaBoost::train\n";

	{
		LOG(INFO) << "Extracting patches for "
							<< training_set.size() << " samples at " 
							<< candidate_features.size() << " candidate features \n";
		const int num_threads = ::utils::system::get_available_logical_cpus();
		boost::thread_pool::executor e(num_threads);
		for (size_t i = 0; i < training_set.size(); ++i) {
			e.submit(boost::bind(&Sample::extract_patches,
													 boost::ref(training_set[i]),
													 candidate_features));
		}
		e.join_all();
	}

	// Split samples.
	vector<Sample*> pos_samples, neg_samples;
	split_samples(training_set, &pos_samples, &neg_samples);
	CHECK(pos_samples.size() >= param_.sampled_count);
	CHECK(neg_samples.size() >= param_.sampled_count);

	// Initialize weights.
	vector<double> pos_weights(pos_samples.size(), 1.0 / pos_samples.size());
	vector<double> neg_weights(neg_samples.size(), 1.0 / neg_samples.size());

	double prev_score = 0.0;
	for (size_t t = 0; t < param_.rounds; ++t) {
		LOG(INFO) << "Boosting round " << t << "\n";

		// Sample active subset.
		vector<Sample*> sampled;
		sample_subset(pos_samples, param_.sampled_count, pos_weights, *gen,
									&sampled);
		sample_subset(neg_samples, param_.sampled_count, neg_weights, *gen,
									&sampled);

		// Pick classifier that is best when combined with the current model.
		WeakClassifier<Sample, Feature> best;
		const double best_score = best_classifier(candidate_features, sampled,
																							training_set, &best);

		// TODO(gandrada): add stopping criterion.
		
		classifiers_.push_back(best);
		prev_score = best_score;

		// Update weights.
		update_weights(pos_samples, pos_weights);
		update_weights(neg_samples, neg_weights);
	}

	LOG(INFO) << "Backward removing\n";
	backward_removing(training_set);
}

template <class Sample, class Feature>
void AdaBoost<Sample, Feature>::backward_removing(
		const vector<Sample*>& training_set) {
	double prev_score = evaluate(training_set);
	while (true && classifiers_.size() > 1) {
		double best = 0.0;
		size_t best_k = -1;
		for (size_t k = 0; k < classifiers_.size(); ++k) {
			// Compute score without current classifier.
			exclude_classifier_ = k;
			const double score = evaluate(training_set);
			if (score > best) {
				best = score;
				best_k = k;
			}
		}
		// Reset excluded classifier.
		exclude_classifier_ = -1;

		LOG(INFO) << "Best " << best << " vs " << prev_score << "\n";
		if (best >= prev_score && best_k != -1) {
			LOG(INFO) << "Removed weak classifier " << best_k
								<< ". New score " << best << "\n";
			classifiers_.erase(classifiers_.begin() + best_k); 
			prev_score = best;
		} else {
			break;
		}
	}
}

template <class Sample, class Feature>
double AdaBoost<Sample, Feature>::predict(const Sample& sample) const {
	if (classifiers_.size() == 0) {
		return 0;
	}

	double r = 0.0;
	size_t count = 0;
	for (size_t k = 0; k < classifiers_.size(); ++k) {
		if (k == exclude_classifier_) {
			continue;
		}
		++count;
		r += classifiers_[k].predict(sample);
	}
	return r / count;
}

template class AdaBoost<BoostingSample<vision::features::SURFFeature>,
				 								vision::features::SURFFeature>;
template class AdaBoost<BoostingSample<vision::features::T2SURFFeature>,
				 								vision::features::T2SURFFeature>;

} // namespace boosting
} // namespace learning
