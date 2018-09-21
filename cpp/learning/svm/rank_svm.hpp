/*
 * rank_svm.hpp
 *
 * 	Created on: Nov 27, 2013
 * 			Author: gandrada
 *
 * 	Wrapper for dlib's RankSVM implementation.
 *
 */

#ifndef AWESOMENESS__LEARNING_SVM_RANK_SVM_H_
#define AWESOMENESS__LEARNING_SVM_RANK_SVM_H_

#include <vector>

#include <dlib/svm/ranking_tools.h>
#include <dlib/svm/svm.h>
#include <dlib/svm/svm_rank_trainer.h>
#include <opencv2/core/core.hpp>

namespace awesomeness {
namespace learning {
namespace svm {

class RankSvm {
public:
	typedef dlib::matrix<float> sample_type;
	typedef dlib::linear_kernel<sample_type> kernel_type;

	// TODO(gandrada): add params
	RankSvm(double c = 1.0) : c_(c) {
		trainer_.set_c(c_);
	}

	void add_query(const cv::Mat_<float>& f1, const cv::Mat_<float>& f2,
								 bool test = false);

	// Every sample in vector s1 is considered to be more relevant than each
	// sample in s2.
	void add_query(const std::vector<cv::Mat_<float> >& s1,
								 const std::vector<cv::Mat_<float> >& s2, bool test = false);

	void train();

	void test(double* acc, double* mean_ap) const;

	double predict(const cv::Mat_<float>& f) const;

	void serialize(std::ofstream& ofs) {
		dlib::serialize(rank_, ofs);
	}

	void deserialize(std::ifstream& ifs) {
		dlib::deserialize(rank_, ifs);
	}

private:
	double c_;
	std::vector<dlib::ranking_pair<sample_type> > samples_;
	std::vector<dlib::ranking_pair<sample_type> > test_samples_;
	dlib::svm_rank_trainer<kernel_type> trainer_;
	dlib::decision_function<kernel_type> rank_;
};

} // namespace svm
} // namespace learning
} // namespace awesomeness

#endif /* AWESOMENESS__LEARNING_SVM_RANK_SVM_H_ */
