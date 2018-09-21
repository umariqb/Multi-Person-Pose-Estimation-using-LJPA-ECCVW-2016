/*
 * multiclass.hpp
 *
 * 	Created on: Jan 10, 2014
 * 			Author: gandrada
 *
 * 	Wrapper for dlib's multiclass classification implementations. 
 *
 */

#ifndef AWESOMENESS__LEARNING_SVM_MULTICLASS_H_
#define AWESOMENESS__LEARNING_SVM_MULTICLASS_H_

#include <string> 
#include <vector> 

#include <glog/logging.h>

#include <dlib/svm/multiclass_tools.h>
#include <dlib/svm/svm.h>
#include <dlib/svm/svm_c_trainer.h>
#include <dlib/svm/svm_c_linear_trainer.h>
#include <dlib/svm/svm_c_ekm_trainer.h>
#include <dlib/svm/pegasos.h>
#include <dlib/svm/krr_trainer.h>
#include <dlib/svm/rvm.h>
#include <opencv2/core/core.hpp>

namespace awesomeness {
namespace learning {
namespace svm {

class Multiclass {
public:
	typedef dlib::matrix<double> sample_type;
	//typedef std::map<unsigned long, double> sample_type;
	typedef double label_type;
	typedef dlib::linear_kernel<sample_type> kernel_type;
	//typedef dlib::sparse_linear_kernel<sample_type> kernel_type;
	typedef dlib::svm_c_ekm_trainer<kernel_type> small_trainer_type;

	Multiclass(double c = 1.0) : c_(c) {}

	virtual ~Multiclass() {}

	virtual void add_sample(const cv::Mat_<float>& f, const std::string& label);

	virtual void train() = 0;


	virtual std::string classify(const cv::Mat_<float>& f) const;

protected:
	double get_label(const std::string& name);
	
	virtual double get_decision(const sample_type& s) const = 0;
	
	double c_;

	std::vector<sample_type> samples_;
	std::vector<std::string> label_names_;
	std::vector<label_type> labels_;
};

} // namespace svm
} // namespace learning
} // namespace awesomeness

#endif /* AWESOMENESS__LEARNING_SVM_MULTICLASS_H_ */
