#include "dllheader.h"

const std::string CLASSES[] = { "background", "car" };

std::string basename(std::string const& path) {
	return std::string(
		std::find_if(path.rbegin(), path.rend(), [](char ch) { 
			return ch == '\\' || ch == '/';
		}).base(), path.end()
	);
}

LIB_DETECTION torch::Device _stdcall DetectorDevice(const char* filename) {
	std::string file = basename(filename);
	if (file.find("CUDA") != std::string::npos) {
		if (torch::hasCUDA()) {
			return torch::DeviceType::CUDA;
		} else {
			std::cout << "Warning: CUDA not enabled, but weight file compiled by CUDA";
		}
	}
	return torch::DeviceType::CPU;
}


LIB_DETECTION Detector * _stdcall DetectorInit(const char* filename, bool cuda) {
	Detector* handle = new Detector;

	try {
		handle->network = torch::jit::load(filename);
		handle->device = DetectorDevice(filename);
		handle->network.to(handle->device);
		handle->network.eval();
	}
	catch (const c10::Error& e) {
		delete handle;
		return nullptr;
	}

	return handle;
}


extern "C" LIB_DETECTION long _stdcall DetectorRelease(Detector * AHandle) {
	if (!AHandle) return -101;

	delete AHandle;
	AHandle = NULL;
	return 0;
}


extern "C" LIB_DETECTION int _stdcall DetectorClassify(Detector * AHandle, const cv::Mat & AImage) {
	cv::Mat image;
	cv::Size dSize = AHandle->size;

	cv::resize(AImage, image, dSize);
	image.convertTo(image, CV_32FC3, 1 / 255.f);

	std::vector<int64_t> shape{ 1,
		static_cast<int64_t>(image.rows), static_cast<int64_t>(image.cols), static_cast<int64_t>(image.channels()) };
	torch::TensorOptions options(torch::kFloat);
	torch::Tensor image_tensor = torch::from_blob(image.data, torch::IntList(shape), options).permute({ 0, 3, 1, 2 });
	image_tensor = image_tensor.to(AHandle->device);
	image_tensor.set_requires_grad(false);

	std::vector<torch::jit::IValue> inputs{ image_tensor };

	auto output = AHandle->network.forward(inputs).toTensor();
	return torch::argmax(output, 1).item<int>();
}


extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetect(Detector * AHandle, cv::Mat & AImage, bool refine) {
	cv::Mat image;
	cv::Size dSize = AHandle->size;

	cv::resize(AImage, image, dSize);
	image.convertTo(image, CV_32FC3, 1);
	image = image - cv::Scalar(104, 117, 123);

	std::vector<int64_t> shape{ 1,
		static_cast<int64_t>(image.rows), static_cast<int64_t>(image.cols), static_cast<int64_t>(image.channels()) };
	torch::TensorOptions options(torch::kFloat32);
	torch::Tensor image_tensor = torch::from_blob(image.data, torch::IntList(shape), options).permute({ 0, 3, 1, 2 });
	image_tensor = image_tensor.to(AHandle->device);
	image_tensor.set_requires_grad(false);

	std::vector<torch::jit::IValue> inputs{ image_tensor };

	auto output = AHandle->network.forward(inputs).toTensor().to(torch::kCPU);
	auto results = new std::vector<Box>();
	int batch_size = output.size(0), class_size = output.size(1);

	for (int batch = 0; batch < batch_size; batch++) {
		for (int klass = 1; klass < class_size; klass++) {
			auto result = output[batch][klass];

			// masking confidence threshold
			result = result.index_select(0, torch::where(
				result.index_select(1, torch::arange(0, 1, torch::kLong)).squeeze() > AHandle->confidence)[0]);
			
			if (refine) {
				// refine boxes using nms
				result = result.index_select(0, nms(
					result.index_select(1, torch::arange(1, 5, torch::kLong)) /* boxes  */,
					result.index_select(1, torch::arange(0, 1, torch::kLong)) /* scores */, AHandle->refine));
			}

			for (int index = 0; index < result.size(0); index++) {
				auto data = static_cast<float*>(result[index].data_ptr());

				if (data[0] > AHandle->confidence) {
					results->push_back({
						data[1] * AImage.cols, data[2] * AImage.rows,
						data[3] * AImage.cols, data[4] * AImage.rows,
						data[0], static_cast<unsigned int>(index),
					});
				}
			}
		}
	}

	return results;
}


extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetectImage(Detector * AHandle, const char* filename) {
	if (!AHandle)
		return nullptr;

	cv::Mat image = cv::imread(cv::String(filename));

	return DetectorDetect(AHandle, image);
}


extern "C" LIB_DETECTION  std::vector<Box> * _stdcall DetectorDetectMat(Detector * AHandle, unsigned char* pArray, int nArraySize) {
	if (!AHandle)
		return nullptr;

	std::vector<char> vdata(pArray, pArray + nArraySize);
	cv::Mat image = imdecode(cv::Mat(vdata), 1);

	return DetectorDetect(AHandle, image);
}


extern "C" LIB_DETECTION long _stdcall DetectorSetParam(Detector * AHandle, int DSize_W, int DSize_H, float AConfidenceThreshold, float ARefineThreshold) {
	if (DSize_W && DSize_H) {
		AHandle->size = cv::Size(DSize_W, DSize_H);
	}

	if (AConfidenceThreshold) {
		AHandle->confidence = AConfidenceThreshold;
	}
	if (ARefineThreshold) {
		AHandle->refine = ARefineThreshold;
	}

	return 0;
}


extern "C" LIB_DETECTION long _stdcall DetectorGetObjectString(int AObj_idx, char* AObjName, int AObjSize) {
	int tClassCount = sizeof(CLASSES) / sizeof(std::string);
	if (AObj_idx < 0 || AObj_idx >= tClassCount) return -1;
	if (!AObjName || AObjSize <= 0) return -1;

	strcpy_s(AObjName, AObjSize, CLASSES[AObj_idx].c_str());
	return 0;
}
