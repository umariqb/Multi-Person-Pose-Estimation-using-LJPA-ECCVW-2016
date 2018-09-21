/*
 * struct_svm.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: lbossard
 */

#include "struct_svm.hpp"

namespace awesomeness {
namespace learning {
namespace svm {



} /* namespace svm */
} /* namespace learning */
} /* namespace awesomeness */

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(awesomeness::learning::svm::SparseStructSvm);
BOOST_CLASS_EXPORT(awesomeness::learning::svm::SparseStructSvmFloat);
BOOST_CLASS_EXPORT(awesomeness::learning::svm::DenseStructSvm);
BOOST_CLASS_EXPORT(awesomeness::learning::svm::DenseStructSvmFloat);
