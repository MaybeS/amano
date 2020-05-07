#include <string>
#include <vector>

#include "LibDetector.h"
#include "timer.h"

const std::string IMAGE{ "../../testset/images/source.jpg" };
const std::string MODEL{ "../../testset/SSD-VGG16-mAP95.pt" };

int main() {
	cv::Mat image = cv::imread(IMAGE);
	Detector* mHandle = DetectorInit(MODEL.c_str(), 1);
	DetectorSetParam(mHandle, 300, 300);

	measure<>::logging(measure<>::execution([&mHandle, &image]() {
		auto results = DetectorDetect(mHandle, image);
	}), "MN Detection");
}
