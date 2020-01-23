#include <string>
#include <vector>

#include "detector.h"
#include "timer.h"

const std::string IMAGE{ "./bin/test.jpg" };

int main() {
	MNHandle* mHandle = nullptr;
	cv::Mat image;
	int results = 0;

	measure<>::logging(measure<>::execution([&image]() {
		image = cv::imread(IMAGE);
	}), "Image read");

	measure<>::logging(measure<>::execution([&mHandle]() {
		mHandle = MN_Init("./bin/amano-mb2-64.pt", 0);
		MN_SetParam(mHandle, 64, 64);
	}), "MN Init");

	measure<>::logging(measure<>::execution([&mHandle, &image, &results]() {
		results = MN_Classify(mHandle, image);
	}), "MN Detection");
}
