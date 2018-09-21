/*
 * rank_svm.cpp
 *
 * 	Created on: Nov 27, 2013
 * 			Author: gandrada
 *
 * 	Wrapper for dlib's RankSVM implementation.
 *
 */

#include "cpp/learning/svm/rank_svm.hpp"

#include <glog/logging.h>

namespace {

void cvmat_to_dlib(const cv::Mat_<float>& in,
									 dlib::matrix<float>* out) {
	out->set_size(in.cols, 1);
	for (int i = 0; i < in.cols; ++i) {
		(*out)(i) = in.at<float>(0, i);
	}
}

} // namespace

namespace awesomeness {
namespace learning {
namespace svm {

void RankSvm::add_query(const cv::Mat_<float>& f1, const cv::Mat_<float>& f2,
												bool test) {
	RankSvm::sample_type s1, s2;
	cvmat_to_dlib(f1, &s1);
	cvmat_to_dlib(f2, &s2);

	dlib::ranking_pair<dlib::matrix<float> > p;
	p.relevant.push_back(s1);
	p.nonrelevant.push_back(s2);

	if (!test) {
		samples_.push_back(p);
	} else {
		test_samples_.push_back(p);
	}
}

void RankSvm::add_query(const std::vector<cv::Mat_<float> >& s1,
												const std::vector<cv::Mat_<float> >& s2, bool test) {
	LOG(INFO) << "Adding query with relevant: " << s1.size()
						<< " nonrelevant: " << s2.size();

	dlib::ranking_pair<dlib::matrix<float> > p;

	for (int i = 0; i < s1.size(); ++i) {
		RankSvm::sample_type s;
		cvmat_to_dlib(s1[i], &s);
		p.relevant.push_back(s);
	}

	for (int i = 0; i < s2.size(); ++i) {
		RankSvm::sample_type s;
		cvmat_to_dlib(s2[i], &s);
		p.nonrelevant.push_back(s);
	}

	if (!test) {
		samples_.push_back(p);
	} else {
		test_samples_.push_back(p);
	}
}

void RankSvm::train() {
	rank_ = trainer_.train(samples_);
	LOG(INFO) << "Training done.";
	LOG(INFO) << "Training data - ordering accuracy, mean average precision: "
						<< dlib::test_ranking_function(rank_, samples_);
}

void RankSvm::test(double* acc, double* mean_ap) const {
	dlib::matrix<double, 1, 2> r = dlib::test_ranking_function(rank_,
																														 test_samples_);
	*acc = r(0);
	*mean_ap = r(1);
	LOG(INFO) << "Testing data - ordering accuracy, mean average precision: "
						<< r;
}

double RankSvm::predict(const cv::Mat_<float>& f) const {
	RankSvm::sample_type s;
	cvmat_to_dlib(f, &s);
	return rank_(s);
}

} // namespace svm
} // namespace learning
} // namespace awesomeness
