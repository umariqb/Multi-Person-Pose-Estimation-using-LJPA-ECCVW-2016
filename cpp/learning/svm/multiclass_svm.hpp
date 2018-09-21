/*
 * rank_svm.hpp
 *
 * 	Created on: Jan 10, 2014
 * 			Author: gandrada
 *
 * 	Wrapper for dlib's Multiclass SVM implementation.
 *
 */

#ifndef AWESOMENESS__LEARNING_SVM_MULTICLASS_SVM_H_
#define AWESOMENESS__LEARNING_SVM_MULTICLASS_SVM_H_

#include <string> 
#include <vector> 

#include "cpp/learning/svm/multiclass.hpp"
#include <dlib/svm/svm_multiclass_linear_trainer.h>

namespace awesomeness {
namespace learning {
namespace svm {

class MulticlassSvm : public Multiclass {
public:
	MulticlassSvm(double c = 1.0) : Multiclass(c) {
		trainer_.set_c(c_);
	}

	void train() {
		LOG(INFO) << "Training started.";
		trainer_.set_epsilon(0.001);
		decision_ = trainer_.train(samples_, labels_);
		LOG(INFO) << "Training done.";
	}

protected:
	virtual double get_decision(const sample_type& s) const {
		return decision_(s);
	}

private:
	dlib::svm_multiclass_linear_trainer<kernel_type, label_type> trainer_;
	dlib::multiclass_linear_decision_function<kernel_type, label_type> decision_;
};

} // namespace svm
} // namespace learning
} // namespace awesomeness

#endif /* AWESOMENESS__LEARNING_SVM_MULTICLASS_SVM_H_ */
