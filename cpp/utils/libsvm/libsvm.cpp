/*
 * LibSVM.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: lbossard
 */

#include "libsvm.hpp"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <glog/logging.h>

#include "cpp/third_party/libsvm/svm.h"

namespace utils
{
namespace libsvm
{

ProblemHolder::ProblemHolder()
{
    clear();

}
/*virtual*/ ProblemHolder::~ProblemHolder()
{
    try
    {
        this->clear();
    }
    catch(...)
    {}
}


void ProblemHolder::clear()
{
    static const int min_capacity = 10;
    // init memory
    y_.resize(min_capacity);
    x_features_.resize(min_capacity);
    x_.resize(min_capacity, NULL);
    problem_.y = y_.data();
    problem_.x = x_.data();

    // set book keepers for entries
//    problem_.bias = -1;
    problem_.l    = 0; // number of labels
    next_node_idx_ = 0;
}


void ProblemHolder::allocate(const std::size_t sample_count, const std::size_t non_zero_entries)
{
    // first: clear
    clear();

    // allocate the memroy
    y_.resize(sample_count);
    x_.resize(sample_count);
    std::size_t x_element_count_ = non_zero_entries + (2L * sample_count); // all vector elements + evtl. bias term + -1 entry
    x_features_.resize(x_element_count_);

    // set up libsvms problem
    problem_.l = 0; // number of labels go here
   // problem_.n = 0; // number of max feature dimensions go here (note: this is +1 if with bias term, but we dont support this)
    problem_.y = y_.data();
    problem_.x = x_.data();
}


bool ProblemHolder::read(std::istream& stream)
{
    std::streampos start_position = stream.tellg();

    // count lines and non_zero_elements
    std::size_t number_of_features = 0;
    std::size_t number_of_elements = 0;
    {
        // line count is the number of \n new line chars
        // and the number of features are the occurrences of ':'
        const std::istreambuf_iterator<char> eos;
        std::istreambuf_iterator<char> it(stream.rdbuf());
        while (it != eos)
        {
            switch (*it++) {
                case ':':
                    ++number_of_elements;
                    break;
                case '\n':
                    ++number_of_features;
                    break;
                default:
                    break;
            };
        }
        // go back to start
        stream.seekg(start_position, std::ios::beg);
    }

    // allocate the memroy
    this->allocate(number_of_features, number_of_elements);

    // read the vectors
    {
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;

        const Separator element_separator(" \t");
        const Separator item_separator(":");

        std::size_t feature_count = 0;
        int max_idx = 0;
        std::string line;
        while (std::getline(stream, line))
        {
            Tokenizer element_tokenizer(line, element_separator);
            Tokenizer::iterator elem_it = element_tokenizer.begin();

            // first number is the label
            int label_id = boost::lexical_cast<int>(*elem_it++);
            y_[feature_count] = label_id;
            x_[feature_count] = &x_features_[next_node_idx_];

            // read the entries
            for(; elem_it != element_tokenizer.end(); ++elem_it)
            {
                Tokenizer item_tokenizer(*elem_it, item_separator);
                Tokenizer::iterator elem_itr = item_tokenizer.begin();
                int index = boost::lexical_cast<int>(*elem_itr++);
                double value = boost::lexical_cast<double>(*elem_itr);

                if (next_node_idx_ >= x_features_.size())
                {
                    LOG(ERROR) << "Reading more elements, than previously counted. aborting" << std::endl;
                    return false;
                }
                FeatureNode& node = x_features_[next_node_idx_++];
                node.index = index;
                node.value = value;
                max_idx = std::max(index, max_idx);
            }

            if (next_node_idx_ >= x_features_.size())
            {
                LOG(ERROR) << "Reading more elements, than previously counted. aborting" << std::endl;
                return false;
            }
            x_features_[next_node_idx_++].index = -1;
            ++feature_count;

            if (feature_count > number_of_features)
            {
                LOG(ERROR) << "Reading more features, than previously counted. aborting" << std::endl;
                return false;
            }
        }
        problem_.l = feature_count;
        //problem_.n = max_idx;
    }

    return true;
}


namespace {
  std::vector<double> init_default_costs(){
    std::vector<double> default_costs;
    default_costs.push_back(0.0001);
    default_costs.push_back(0.001);
    default_costs.push_back(0.01);
    default_costs.push_back(0.1);
    default_costs.push_back(1);
    default_costs.push_back(10);
    default_costs.push_back(100);
    default_costs.push_back(1000);
    return default_costs;
  }

  struct MallocDeleter
  {
      void operator()(void* x)
      {
          free(x);
      }
  };
}
/*static*/ const std::vector<double> SVMHolder::default_cost = init_default_costs();

SVMHolder::SVMHolder()
{
   model_.reset(new Model());
  // model_->bias = -1;
   model_->label = NULL;
   model_->nr_class = 0;
  // model_->nr_feature = 0;
  // model_->w = NULL;
   model_->param.nr_weight = 0;
}

/*virtual*/ SVMHolder::~SVMHolder()
{
    clear();

}

void SVMHolder::clear()
{
    //DONT calls: free_model_content(model_.get());
    // we manage the labels on our own
    weights_.clear();
    labels_.clear();
    model_.reset();
}

bool SVMHolder::read_from_text(const std::string& file)
{
    clear();
    model_.reset(wrapped::svm_load_model(file.c_str()), MallocDeleter());

    if (!model_)
    {
        return false;
    }
    return true;
}

int SVMHolder::get_num_weights() const {
//    int class_count = model_->nr_class;
//  //  int feature_dim = model_->nr_feature;
//    int feature_dim = model_->param.nr_weight;
//    if (model_->bias >= 0)
//    {
//        ++feature_dim;
//    }
//    // if only two classes: we need to store just one svm
//    if (class_count == 2 && model_->param.solver_type != solver_type::MCSVM_CS)
//    {
//        class_count = 1;
//    }

//    return class_count * feature_dim;
  model_->param.nr_weight;
}

bool SVMHolder::write_as_text(const std::string& file) const
{
    return 0 == wrapped::svm_save_model(file.c_str(), model_.get());
}

int SVMHolder::predict(const cv::Mat_<int>& vec, const cv::Mat_<uchar>& mask)  const
{
    std::vector<double> values;
    return predict(vec, values, mask);
}

int SVMHolder::predict_probability(const cv::Mat_<int>& vec, const cv::Mat_<uchar>& mask)  const
{
    std::vector<double> values;
    return predict_probability(vec, values, mask);
}

int SVMHolder::predict(const std::vector<FeatureNode>& features, std::vector<double>& values) const
{
    return predict(features.data(), values);
}

int SVMHolder::predict_probability(const std::vector<FeatureNode>& features, std::vector<double>& values) const
{
  return predict_probability(features.data(), values);
}


int SVMHolder::predict(const FeatureNode* feature, std::vector<double>& values) const
{
  values.resize(model_->nr_class);
  int class_label =  wrapped::svm_predict_values(model_.get(), feature, values.data());
  if (model_->nr_class == 2){
    values[1] = - values[0];
  }
  return class_label;
}

int SVMHolder::predict_probability(const FeatureNode* feature, std::vector<double>& values) const
{
  values.resize(model_->nr_class);
  int class_label =  wrapped::svm_predict_probability(model_.get(), feature, values.data());
//  if (model_->nr_class == 2){
//    values[1] = - values[0];
//  }
  return class_label;
}

void SVMHolder::setDefaults(SolverParameter& param)
{
    if (param.eps <= 0)
    {
        switch (param.svm_type)
        {
        case svm_type::C_SVC:
        case svm_type::NU_SVC:
        case svm_type::ONE_CLASS:
        case svm_type::EPSILON_SVR:
        case svm_type::NU_SVR:
        default:
          break;
        }
    }

    if (param.nr_weight == 0)
    {
        param.weight = NULL;
        param.weight_label = NULL;
    }

    param.probability = 1;
}

float SVMHolder::cross_validation(
        int num_folds,
        const ProblemHolder& problem,
        const SolverParameter param_)
{
    const int example_count = problem.libsvm_problem().l;
    std::vector<double> target(example_count, 0);

    wrapped::svm_cross_validation(
        &problem.libsvm_problem(),
        &param_,
        num_folds,
        &target[0]);

    int total_correct = 0;
    for(int i = 0; i < example_count; ++i){
        if(target[i] == problem.libsvm_problem().y[i]){
          ++total_correct;
        }
    }
    return static_cast<float>(total_correct) / example_count;
}


void SVMHolder::train(
        const ProblemHolder& problem,
        SolverParameter param_)
{
    // train and save our model
    clear();
    model_.reset(wrapped::svm_train(&problem.libsvm_problem(), &param_), MallocDeleter());
}

void SVMHolder::train_best_model(
    const ProblemHolder& svm_problem,
    const SolverParameter param_,
    const std::vector<double>& costs,
    const int32_t fold_count
){
  const double eps = 0.;
  float best_accuracy = -1.f;
  SolverParameter best_param;

  LOG(INFO) << "searching parameters with "<< fold_count  << "-fold cross valiation";
  const std::size_t num_costs = costs.size();
  for (int i = 0; i <= 5; ++i){
    for (std::size_t c_idx = 0; c_idx < num_costs; ++c_idx){
      const double cost = costs[c_idx];
      svm_type::T loss_type = static_cast<svm_type::T>(i);
      SolverParameter param = param_;
      param.svm_type = loss_type;
      param.C = cost;
      param.eps = eps;
      float accuracy =  this->cross_validation(fold_count, svm_problem, param);
      if (accuracy > best_accuracy){
        best_param = param;
      }
    }
  }
  LOG(INFO) << "train model with best found parameters (accuracy=" <<
                best_accuracy   << ", loss="<< best_param.svm_type <<
                ", c="<< best_param.C << ")";

  this->train(svm_problem, best_param);
}

void SVMHolder::own_model_from_libsvm() {

  labels_.resize(model_->nr_class);
  std::copy(model_->label, model_->label + model_->nr_class, labels_.begin() );

//  int num_weights = get_num_weights();
//  weights_.resize(num_weights);
  //std::copy(model_->w, model_->w + num_weights, weights_.begin() );

  //svm_free_model_content(model_.get());
  model_->label = const_cast<int*>(labels_.data());
  //model_->w = const_cast<double*>(weights_.data());

}

void ProblemHolder::dynamic_expand_features() {
//  LOG(INFO) << "Reading more elements, than previously counted. re-allocating (" << x_features_.size() << " )" << std::endl;

  // save away diffs
  const std::size_t sample_count = problem_.l + 1;
  std::vector<std::ptrdiff_t> indexes(sample_count, 0);
  const FeatureNode* base = &x_features_[0];
  for (int i = 0; i <  sample_count; ++i){
    indexes[i] = x_[i] - base;
  }
  // resize vector to double capacity
  x_features_.resize(2 * x_features_.size());
  // rebuild diff info
  const FeatureNode* new_base = &x_features_[0];
  for (int i = 0; i < sample_count; ++i){
    x_[i] = const_cast<FeatureNode*>(new_base + indexes[i]);
  }
}

void ProblemHolder::dynamic_expand_labels() {
//  LOG(INFO) << "Reading more samples, than previously counted. re-allocating" << std::endl;

  y_.resize(y_.size() * 2);
  x_.resize(x_.size() * 2);

  problem_.y = y_.data();
  problem_.x = x_.data();
}

SolverParameter get_default_svm_param()
{
  SolverParameter param;
  param.svm_type        = svm_type::C_SVC;
  param.kernel_type     = kernel_type::RBF;
  param.degree          = 0;
  param.gamma           = 1;
  param.coef0           = 0;
  param.C               = 1;
  param.nu              = 0;
  param.p               = 0;
  param.cache_size      = 500;
  param.nr_weight       = 0;		/* for C_SVC */
  param.weight_label    = NULL;
  param.weight          = NULL;
  param.shrinking       = 1;
  param.probability     = 1;
  param.eps             = FLT_EPSILON;
  return param;
}


} /* namespace libsvm */
} /* namespace utils */
