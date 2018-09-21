/*
 * poselets.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: mdantone
 */


#include "poselets.hpp"
namespace body_pose {
namespace poselets {

bool save_poselet(const Poselet& poselet, const std::string save_path) {
  return utils::serialization::write_binary_archive(save_path, poselet);
}

bool load_poselet(const std::string save_path, Poselet& poselet) {
  return utils::serialization::read_binary_archive(save_path, poselet);
}

}
}

