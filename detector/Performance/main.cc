#include <string>
#include <vector>

#include "detector.h"
#include "timer.h"

const std::string IMAGE{ "./bin/test.jpg" };

int main() {
	Detector* mHandle = nullptr;
	cv::Mat image;
	int results = 0;

	measure<>::logging(measure<>::execution([&image]() {
		image = cv::imread(IMAGE);
	}), "Image read");

	measure<>::logging(measure<>::execution([&mHandle]() {
		mHandle = DetectorInit("./bin/amano-mb2-128.pt", 0);
		DetectorSetParam(mHandle, 128, 128);
	}), "MN Init");

	measure<>::logging(measure<>::execution([&mHandle, &image, &results]() {
		results = DetectorClassify(mHandle, image);
	}), "MN Detection");
}
