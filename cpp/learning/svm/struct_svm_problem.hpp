/*
 * struct_svm_problem.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#ifndef AWESOMENESS__LEARNING__SVM__STRUCT_SVM_PROBLEM_HPP_
#define AWESOMENESS__LEARNING__SVM__STRUCT_SVM_PROBLEM_HPP_

#include "struct_svm.hpp"

#ifdef DLIB_USE_BLAS
extern "C" void openblas_set_num_threads(int num_threads);
#endif

namespace awesomeness {
namespace learning {
namespace svm {

struct StructSvmParameters : public SvmParameters {
  virtual ~StructSvmParameters(){};

  // structsvm options
  uint32_t num_threads;
  double C;
  double eps;
  bool is_sparse;
  bool is_verbose;

  // oca options
  double oca_sub_eps;
  unsigned long oca_sub_max_iter;
  unsigned long oca_inactive_thresh;

  StructSvmParameters(){
    C =10;
    eps = 1e-4;
    num_threads = 1;
    is_sparse = true;
    is_verbose = false;

    oca_sub_eps = eps;
    oca_inactive_thresh = 50;
    oca_sub_max_iter = 50000;
  }
};


template <typename StructSvmTrait>
class StructSvmProblem : public SvmProblem {

public:

  typedef StructSvm<StructSvmTrait> SvmType;

  typedef typename SvmType::SampleType SampleType;
  typedef typename SvmType::ScalarType ScalarType;
  typedef std::vector<typename Svm::LabelIndexType> LabelVector;
  typedef std::vector<SampleType> SampleVector;

  StructSvmProblem(
      uint32_t num_classes,
      std::size_t feature_dimensions,
      const StructSvmParameters& svm_parameters);

  virtual ~StructSvmProblem();

  virtual void push_sample(const cv::Mat_<float>& row, const int32_t y);
  virtual Svm* train(std::vector<int> weight_label = std::vector<int>(),
      std::vector<double> weights = std::vector<double>());

  template<typename T>
  void push_sample(const cv::Mat_<T>& row, const int32_t y);

  inline const SampleVector& samples() const {return _samples;};
  inline const LabelVector& labels() const {return _labels;};
  inline const StructSvmParameters& parameters() const { return _params;}

  virtual void clear_samples(){
    _labels.clear();
    _samples.clear();
  }

  virtual std::size_t num_samples() const {
    return _labels.size();
  }

private:
  StructSvmProblem();

  virtual void allocate(std::size_t sample_count, std::size_t nonzerohint=0);

  StructSvmParameters _params;
  LabelVector _labels;
  SampleVector _samples;
};

typedef StructSvmProblem<SparseStructSvmTrait> SparseStructSvmProblem;
typedef StructSvmProblem<DenseStructSvmTrait> DenseStructSvmProblem;

typedef StructSvmProblem<SparseStructSvmTrait> SparseStructSvmProblemFloat;
typedef StructSvmProblem<DenseStructSvmTrait> DenseStructSvmProblemFloat;

////////////////////////////////////////////////////////////////////////////////

template <typename StructSvmTrait>
struct _DlibStructProblem :
//#ifdef AWESOMENESS_DEBUG
//  public  dlib::structural_svm_problem<typename StructSvm<StructSvmTrait>::Weight, std::vector<std::pair<uint32_t, typename StructSvmTrait::ScalarType> > > {
//  typedef dlib::structural_svm_problem<typename StructSvm<StructSvmTrait>::Weight, std::vector<std::pair<uint32_t, typename StructSvmTrait::ScalarType> > > parent_type;
//#else
  public  dlib::structural_svm_problem_threaded<typename StructSvm<StructSvmTrait>::Weight, std::vector<std::pair<uint32_t, typename StructSvmTrait::ScalarType> > > {
  typedef dlib::structural_svm_problem_threaded<typename StructSvm<StructSvmTrait>::Weight, std::vector<std::pair<uint32_t, typename StructSvmTrait::ScalarType> > > parent_type;
//#endif

  typedef StructSvmProblem<StructSvmTrait> Problem;
  typedef typename StructSvmTrait::ScalarType ScalarType;
  typedef typename StructSvm<StructSvmTrait>::Weight Weight;
  typedef std::vector<std::pair<uint32_t, typename StructSvmTrait::ScalarType> > FeatureVectorType;

  const Problem& _problem;
  const uint32_t feature_dim;
  const uint32_t num_classes;

  _DlibStructProblem(const Problem& problem)
  :
//#ifndef AWESOMENESS_DEBUG
    parent_type(problem.parameters().num_threads),
//#endif
    _problem(problem),
    feature_dim(problem.feature_dimensions() + 1), // +1 for bias
    num_classes(problem.num_classes())

  {
    const StructSvmParameters& opts = problem.parameters();
    this->set_max_cache_size(0);
    this->set_c(opts.C);
    this->set_epsilon(opts.eps);

  }

  virtual ~_DlibStructProblem(){

  }

  virtual long get_num_dimensions() const {
    return feature_dim * num_classes;
  }

  virtual long get_num_samples() const {
    return static_cast<long>(_problem.samples().size());
  }

  virtual void get_truth_joint_feature_vector (
      long idx,
      FeatureVectorType& psi
  ) const
  {
    const typename Problem::LabelVector& labels = _problem.labels();
    const typename Problem::SampleVector& samples = _problem.samples();
    psi.clear();
    dlib::assign(psi, samples[idx]);
    // Add a constant -1 to account for the bias term.
    psi.push_back(std::make_pair(feature_dim -1,static_cast<ScalarType>(-1)));

    offset_feature_vector(psi, (unsigned long int)feature_dim * labels[idx]);
  }

  virtual void separation_oracle(const long idx,
      const Weight& current_solution,
      ScalarType& loss,
      FeatureVectorType& psi) const
  {
    const typename Problem::LabelVector& labels = _problem.labels();
    const typename Problem::SampleVector& samples = _problem.samples();

    ScalarType best_val = -std::numeric_limits<ScalarType>::infinity();
    unsigned long best_idx = 0;

    // Figure out which label is the best.  That is, what label maximizes
    // LOSS(idx,y) + F(x,y).  Note that y in this case is given by distinct_labels[i].
    for (uint32_t i = 0; i < num_classes; ++i) {

      // Compute the F(x,y) part:
      // perform: temp == dot(relevant part of current solution, samples[idx]) - current_bias
      ScalarType temp = dlib::dot(
          dlib::rowm(current_solution, dlib::range(i * feature_dim, (i + 1) * feature_dim - 2)),
          samples[idx])
      - current_solution((i + 1) * feature_dim - 1);

      // Add the LOSS(idx,y) part:
      if (labels[idx] != i){
        temp += 1;
      }

      // Now temp == LOSS(idx,y) + F(x,y).  Check if it is the biggest we have seen.
      if (temp > best_val) {
        best_val = temp;
        best_idx = i;
      }
    }

    psi.clear();
    dlib::assign(psi, samples[idx]);
    // add a constant -1 to account for the bias term
    psi.push_back(std::make_pair(feature_dim - 1, static_cast<ScalarType>(-1)));

    offset_feature_vector(psi, feature_dim * best_idx);

    if (best_idx == labels[idx]) {
      loss = 0;
    }
    else {
      loss = 1;
    }

  }

  void offset_feature_vector(
      FeatureVectorType& feature,
      const unsigned long val) const {

    if (val != 0) {
      for (typename FeatureVectorType::iterator i = feature.begin();
          i != feature.end(); ++i) {
        i->first += val;
      }
    }
  }
};


////////////////////////////////////////////////////////////////////////////////

template <typename T>
template<typename TT>
void StructSvmProblem<T>::push_sample(const cv::Mat_<TT>& row, const int32_t y) {
  CHECK(y < _num_classes);
  _labels.push_back(y);
  // add another sample and get its reference
  _samples.resize(_samples.size() + 1);
  SampleType& sample = *_samples.rbegin();

  dlib_helpers::copy_to_sample(row, &sample);
}


template <typename T>
StructSvmProblem<T>::StructSvmProblem(
    uint32_t num_classes,
    std::size_t feature_dimensions,
    const StructSvmParameters& params)
:
   SvmProblem(num_classes, feature_dimensions),
  _params(params)
{
}

template <typename T>
StructSvmProblem<T>::StructSvmProblem():
   SvmProblem(0, 0)
{
}

template <typename T>
StructSvmProblem<T>::~StructSvmProblem(){

}

template <typename T>
/*virtual*/ void StructSvmProblem<T>::allocate(std::size_t sample_count, std::size_t /*nonzerohint*/){
  _samples.reserve(sample_count);
  _labels.reserve(sample_count);
}

template <typename T>
/*virtual*/  void StructSvmProblem<T>::push_sample(const cv::Mat_<float>& row, const int32_t y){
  this->push_sample<float>(row, y);
}

template <typename T>
/*virtual*/ Svm* StructSvmProblem<T>::train(std::vector<int> weight_label,
    std::vector<double> weights) {

  // prepare solver
  dlib::oca solver;
  solver.set_inactive_plane_threshold(_params.oca_inactive_thresh);
  solver.set_subproblem_epsilon(_params.oca_sub_eps);
  solver.set_subproblem_max_iterations(_params.oca_sub_max_iter);

  // solve
  _DlibStructProblem<T> struct_problem(*this);
  if (_params.is_verbose){
    struct_problem.be_verbose();
  }

  typename SvmType::Weight svm_weights;
  {
#ifdef DLIB_USE_BLAS
    openblas_set_num_threads(1);
#endif
    solver(struct_problem, svm_weights);
  }

  // create struct_svm object
  std::auto_ptr<SvmType > struct_svm(new SvmType(_feature_dimensions, _label_idx_map));
  typename SvmType::LinearStructSvm& svm = struct_svm->mutable_svm();

  // create (integer)labels
  const std::size_t class_count = _num_classes;
  svm.labels.resize(class_count);
  for (int i = 0; i < class_count; ++i){
    svm.labels[i] = i;
  }

  // copy weights
  svm.weights = dlib::colm(
      dlib::reshape(svm_weights, struct_problem.num_classes, _feature_dimensions + 1),
      dlib::range(0, _feature_dimensions - 1));
  svm.b = dlib::colm(
      dlib::reshape(svm_weights, struct_problem.num_classes, _feature_dimensions + 1),
      _feature_dimensions);
//  std::cout << svm_weights << std::endl << "--" << std::endl
//      << svm.weights << std::endl<< "--" << std::endl
//      << svm.b << std::endl;

  CHECK(struct_svm->feature_dimension() == svm.weights.nc());
  CHECK(struct_svm->label_idx_map().size() == svm.b.nr());

  return struct_svm.release();
}

} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */
#endif /* AWESOMENESS__LEARNING__SVM__STRUCT_SVM_PROBLEM_HPP_ */
