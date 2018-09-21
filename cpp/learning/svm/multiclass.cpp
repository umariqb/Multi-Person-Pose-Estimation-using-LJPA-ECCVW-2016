/*
 * multiclass.cpp
 *
 * 	Created on: Jan 10, 2014
 * 			Author: gandrada
 *
 * 	Wrapper for dlib's multiclass classification implementations. 
 *
 */

#include "cpp/learning/svm/multiclass.hpp"

#include <glog/logging.h>

namespace {

//void cvmat_to_dlib(const cv::Mat_<float>& in,
									 //awesomeness::learning::svm::Multiclass::sample_type* out) {
	//for (int i = 0; i < in.cols; ++i) {
		//const double c = static_cast<double>(in.at<float>(0, i));
		//if (fabs(c) > 0.005) {
			//(*out)[i] = c;
		//}
	//}
//}

void cvmat_to_dlib(const cv::Mat_<float>& in,
									 awesomeness::learning::svm::Multiclass::sample_type* out) {
	out->set_size(in.cols, 1);
	for (int i = 0; i < in.cols; ++i) {
		(*out)(i) = in.at<float>(0, i);
	}
}

} // namespace

namespace awesomeness {
namespace learning {
namespace svm {

double Multiclass::get_label(const std::string& name) {
	for (int i = 0; i < label_names_.size(); ++i) {
		if (label_names_[i] == name) {
			return static_cast<double>(i);
		}
	}
	label_names_.push_back(name);
	return static_cast<double>(label_names_.size() - 1);
}

void Multiclass::add_sample(const cv::Mat_<float>& f,
															 const std::string& label) {
	Multiclass::sample_type s;
	cvmat_to_dlib(f, &s);
	samples_.push_back(s);
	labels_.push_back(get_label(label));
}

std::string Multiclass::classify(const cv::Mat_<float>& f) const {
	Multiclass::sample_type s;
	cvmat_to_dlib(f, &s);
	const int id = static_cast<int>(get_decision(s));
	if (id < 0 || id >= label_names_.size()) {
		return "error";
	} else {
		return label_names_[id];
	}
}

} // namespace svm
} // namespace learning
} // namespace awesomeness
