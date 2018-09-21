/*
 * test_svm.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */
#include "cpp/third_party/gtest/gtest.h"

#include <dlib/svm.h>
#include <dlib/svm/svm_multiclass_linear_trainer.h>

#include "cpp/learning/svm/rank_svm.hpp"
#include "cpp/learning/svm/struct_svm.hpp"
#include "cpp/learning/svm/struct_svm_problem.hpp"
#include "cpp/learning/svm/struct_svm.hpp"
#include "cpp/learning/svm/liblinear_svm_problem.hpp"
#include "cpp/learning/svm/vl_svm_problem.hpp"
#include "cpp/learning/svm/vl_svm.hpp"
#include "cpp/learning/svm/dlib_linear_svm_problem.hpp"
#include "cpp/learning/svm/dlib_linear_svm.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace awesomeness::learning::svm;


/**
 * one class per dimension
 * @param num_samples
 * @param rnd
 * @param p
 */
void make_dataset(int num_samples, dlib::rand& rnd, SvmProblem* p) {
  for (int i = 0; i < p->num_classes(); ++i) {
    for (int j = 0; j < num_samples / p->num_classes(); ++j) {
      cv::Mat_<float> samp(1, p->feature_dimensions());
      samp = 0;
      samp(i) = rnd.get_random_double();

      p->push_sample(samp, i % p->num_classes());
    }
  }
}

TEST(SvmTest, TestSparseSvmStruct) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  StructSvmParameters p;
  p.C = 1;
  p.eps = 1e-4;
  p.is_sparse = true;
  SparseStructSvmProblem* problem = (SparseStructSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);


  dlib::svm_multiclass_linear_trainer<SparseStructSvm::StructSvmKernel, int32_t> trainer1;
  SparseStructSvm::LinearStructSvm df1;
  trainer1.set_epsilon(1e-4);
  trainer1.set_c(1);
  df1 = trainer1.train(problem->samples(), problem->labels());

  SparseStructSvm* svm = dynamic_cast<SparseStructSvm*>(problem->train());
  CHECK_NOTNULL(svm);

  std::cout << df1.weights << std::endl;
  std::cout << svm->svm().weights << std::endl;


  CHECK_LT(dlib::max(dlib::abs(df1.weights - svm->svm().weights)), 1e-2);
  CHECK_LT(dlib::max(dlib::abs(df1.b - svm->svm().b)), 1e-2);

  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, ((Svm*)svm)->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm->predict(x));
}

TEST(SvmTest, TestDenseSvmStruct) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  StructSvmParameters p;
  p.C = 1;
  p.eps = 1e-4;
  p.is_sparse = false;
  DenseStructSvmProblem* problem = (DenseStructSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);


  dlib::svm_multiclass_linear_trainer<DenseStructSvm::StructSvmKernel, int32_t> trainer1;
  DenseStructSvm::LinearStructSvm df1;
  trainer1.set_epsilon(1e-4);
  trainer1.set_c(1);
  df1 = trainer1.train(problem->samples(), problem->labels());
  std::cout << "foo-1" << std::endl;
  DenseStructSvm* svm = dynamic_cast<DenseStructSvm*>(problem->train());
  CHECK_NOTNULL(svm);
  std::cout << "foo1" << std::endl;

  std::cout << df1.weights << std::endl;
  std::cout << svm->svm().weights << std::endl;


  CHECK_LT(dlib::max(dlib::abs(df1.weights - svm->svm().weights)), 1e-2);
  std::cout << "foo2" << std::endl;
  CHECK_LT(dlib::max(dlib::abs(df1.b - svm->svm().b)), 1e-2);
  std::cout << "foo3" << std::endl;

  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, ((Svm*)svm)->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm->predict(x));
  std::cout << "foo" << std::endl;
}

TEST(SvmTest, TestSimple) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  LibLinearSvmParameters p;
  p.C = 1;
  p.eps = 1e-4;
  LibLinearSvmProblem* problem = (LibLinearSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);

  LibLinearSvm* svm = (LibLinearSvm*)problem->train();

  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, svm->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm->predict(x));

//  std::cout << df1.weights << std::endl;
//  std::cout << svm->svm().weights << std::endl;
}


TEST(SvmTest, TestStructSerialization) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  StructSvmParameters p;
  p.C = 1;
  p.eps = 1e-4;
  p.is_sparse = false;
  DenseStructSvmProblem* problem = (DenseStructSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);

  Svm* svm = dynamic_cast<DenseStructSvm*>(problem->train());

  Svm* svm_loaded;
  {
    std::stringstream sstr;
    // write to stream
    {
      boost::archive::binary_oarchive output_archive(sstr);
      output_archive << svm;
    }

    // load from stream
    {
      boost::archive::binary_iarchive input_archive(sstr);
      input_archive >> svm_loaded;
    }
  }

  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, svm_loaded->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm_loaded->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm_loaded->predict(x));
  std::cout << "foo" << std::endl;
}


TEST(SvmTest, TestVlSvm) {
  int num_classes = 2;
  int feature_dims = num_classes;
  int num_samples = 200;
  dlib::rand rnd;

  VlSvmParameters p;
  p.C = .01;
  VlSvmProblem* problem = (VlSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);

  VlSvm* svm = dynamic_cast<VlSvm*>(problem->train());

  cv::Mat_<float> x(1,2);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, svm->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm->predict(x));

//  std::cout << df1.weights << std::endl;
  std::cout << svm->w() << std::endl;
}

TEST(SvmTest, TestRankSvm) {
	RankSvm ranker;

	cv::Mat_<float> s1(1, 1, 1.0);
	cv::Mat_<float> s2(1, 1, 4.0);
	cv::Mat_<float> s3(1, 1, 6.0);
	cv::Mat_<float> s4(1, 1, 8.0);
	ranker.add_query(s1, s2);
	ranker.add_query(s1, s4);
	ranker.add_query(s3, s4);
	ranker.train();

	CHECK_GT(ranker.predict(s1), ranker.predict(s2));
	CHECK_GT(ranker.predict(s1), ranker.predict(s4));
	CHECK_GT(ranker.predict(s3), ranker.predict(s4));
}


TEST(SvmTest, TestDlibLinear) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  DlibLinearSvmParameters p;
  p.solver_type =  DlibLinearSvmParameters::DualCoordinateDescent;
  p.C = 1;
  p.eps = 1e-4;
  p.is_sparse = false;
  p.is_verbose = false;
  DlibLinearSvmProblem* problem = (DlibLinearSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);



  DlibLinearSvm* svm = dynamic_cast<DlibLinearSvm*>(problem->train());
  CHECK_NOTNULL(svm);


  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, ((Svm*)svm)->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm->predict(x));
}

TEST(SvmTest, TestDlibLinearSerialization) {
  int num_classes = 3;
  int feature_dims = num_classes;
  int num_samples = 100;
  dlib::rand rnd;

  DlibLinearSvmParameters p;
  p.C = 1;
  p.eps = 1e-4;
  p.is_sparse = true;
  DlibLinearSvmProblem* problem = (DlibLinearSvmProblem*)SvmProblem::create(p, num_classes, feature_dims, num_samples);

  make_dataset(num_samples, rnd, problem);


  Svm* svm = problem->train();

  Svm* svm_loaded;
  {
    std::stringstream sstr;
    // write to stream
    {
      boost::archive::binary_oarchive output_archive(sstr);
      output_archive << svm;
    }

    // load from stream
    {
      boost::archive::binary_iarchive input_archive(sstr);
      input_archive >> svm_loaded;
    }
  }

  cv::Mat_<float> x(1,3);
  x = 0;
  x(0) = 23;
  CHECK_EQ(0, svm_loaded->predict(x));

  x = 0;
  x(1) = 23;
  CHECK_EQ(1, svm_loaded->predict(x));

  x = 0;
  x(2) = 23;
  CHECK_EQ(2, svm_loaded->predict(x));
  std::cout << "foo" << std::endl;
}
