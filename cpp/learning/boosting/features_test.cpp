/*
 * features_test.cpp
 *
 * Created on: Jun 26, 2013
 * 		Author: Andrada Georgescu
 */

#include <fstream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "cpp/vision/features/simple_feature.hpp"

using vision::features::T2SURFFeature;

DEFINE_string(features_file, "", "Features file");

int main(int argc, char** argv) {
	google::ParseCommandLineFlags(&argc, &argv, true);
	CHECK(!FLAGS_features_file.empty()) << "No features file.";

	std::ifstream in(FLAGS_features_file.c_str());
	int n;
	in >> n;
	std::cout << "Feature count: " << n << "\n";
	for (int i = 0; i < n; ++i) {
		int x, y, w, h, xr, yr;
		in >> x >> y >> w >> h >> xr >> yr;
		std::cout << x << " " << y << " " << w << " " << h << "\n"; 
		T2SURFFeature f(40, 40, cv::Rect(x, y, w, h), xr, yr);
	}
	return 0;
}
