/*
 * dlib_linear_svm.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: lbossard
 */

#include "dlib_linear_svm.hpp"

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
BOOST_CLASS_EXPORT(awesomeness::learning::svm::DlibLinearSvm);
BOOST_CLASS_EXPORT(awesomeness::learning::svm::SparseDlibLinearSvm);
