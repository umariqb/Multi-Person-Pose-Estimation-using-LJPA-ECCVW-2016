/*
 * svm_problem.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#include "svm_problem.hpp"

#include <auto_ptr.h>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "liblinear_svm_problem.hpp"
#include "struct_svm_problem.hpp"
#include "vl_svm_problem.hpp"
#include "dlib_linear_svm_problem.hpp"

#include "cpp/utils/stl_utils.hpp"

namespace awesomeness {
namespace learning {
namespace svm {

SvmParameters::SvmParameters(){

}


/*virtual*/ SvmParameters::~SvmParameters(){

}


/*static*/ SvmProblem* SvmProblem::create(
      const SvmParameters& svm_params,
      const std::vector<std::string>& labels,
      std::size_t feature_dimensions,
      std::size_t sample_count,
      std::size_t total_nonzero_features_hint){

  const uint32_t num_classes = labels.size();

  std::auto_ptr<SvmProblem> svm_problem;
  if (NULL != dynamic_cast<const LibLinearSvmParameters*>(&svm_params)){
    svm_problem.reset(
        new LibLinearSvmProblem(
            num_classes,
            feature_dimensions,
            static_cast<const LibLinearSvmParameters&>(svm_params)));
  }
  else if (NULL != dynamic_cast<const StructSvmParameters*>(&svm_params)){
    const StructSvmParameters& struct_params = static_cast<const StructSvmParameters&>(svm_params);

    if (struct_params.is_sparse){
      svm_problem.reset(
               new SparseStructSvmProblem(
                   num_classes,
                   feature_dimensions,
                   static_cast<const StructSvmParameters&>(struct_params)));
    }
    else
    {
      svm_problem.reset(
                      new DenseStructSvmProblem(
                          num_classes,
                          feature_dimensions,
                          static_cast<const StructSvmParameters&>(struct_params)));
    }
  }
  else if (NULL != dynamic_cast<const VlSvmParameters*>(&svm_params)){
    CHECK_EQ(num_classes, 2) << "VlSvm supports only 2 classes";
    const VlSvmParameters& params = static_cast<const VlSvmParameters&>(svm_params);
    svm_problem.reset(new VlSvmProblem(num_classes, feature_dimensions, params));
  }
  else if (NULL != dynamic_cast<const DlibLinearSvmParameters*>(&svm_params)){
      const DlibLinearSvmParameters& params = static_cast<const DlibLinearSvmParameters&>(svm_params);
      if (params.is_sparse){
        svm_problem.reset(new SparseDlibLinearSvmProblem(num_classes, feature_dimensions, params));
      }
      else {
        svm_problem.reset(new DlibLinearSvmProblem(num_classes, feature_dimensions, params));
      }
    }
  else{
    CHECK(false) << "Unknown SvmProblem type";
  }

  svm_problem->create_label_idx_map(labels);
  svm_problem->allocate(
      sample_count,
      total_nonzero_features_hint);
  return svm_problem.release();
}

/*static*/ SvmProblem* SvmProblem::create(
    const SvmParameters& svm_params,
    uint32_t num_classes,
    std::size_t feature_dimensions,
    std::size_t sample_count,
    std::size_t total_nonzero_features_hint
    ){

  std::vector<std::string> labels;
  for (uint32_t i = 0; i < num_classes; ++i){
    labels.push_back(boost::lexical_cast<std::string>(i));
  }
  return create(svm_params, labels, feature_dimensions, sample_count, total_nonzero_features_hint);
}


SvmProblem::SvmProblem(){
  _num_classes = 0;
  _feature_dimensions = 0;
}

SvmProblem::SvmProblem(uint32_t num_classes, std::size_t feature_dimensions){
  _num_classes = num_classes;
  _feature_dimensions = feature_dimensions;
}

SvmProblem::~SvmProblem(){

}

void SvmProblem::create_label_idx_map(const std::vector<std::string>& labels_){
  // sort labels alphabetically
  std::vector<std::string> labels = labels_; // copy
  std::sort(labels.begin(), labels.end());
  // create map
  _label_idx_map.clear();
  utils::stl::vector_to_index(labels, _label_idx_map);
}


void SvmProblem::push_sample(const cv::Mat_<float>& row, const std::string& label){
  return push_sample(row, _label_idx_map.at(label));
}


} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
