#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

#ifdef BUILD_DLL
	#define LIB_DETECTION __declspec(dllexport)
#else
	#define LIB_DETECTION __declspec(dllimport)
#endif

struct LIB_DETECTION Box {
	float x, y, w, h;
	float prob;
	unsigned int obj_id;
	unsigned int track_id;
	unsigned int frames_counter;
};

class LIB_DETECTION Detector;

extern "C" LIB_DETECTION Detector * _stdcall DetectorInit(const char* filename, int AGpu);
extern "C" LIB_DETECTION long _stdcall DetectorRelease(Detector * detector);
extern "C" LIB_DETECTION int _stdcall DetectorClassify(Detector * detector, const cv::Mat & AImage);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetect(Detector * detector, cv::Mat & AImage, bool refine = true);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetectImage(Detector * detector, const char* filename);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetectMat(Detector * detector, unsigned char* pArray, int nArraySize);
extern "C" LIB_DETECTION long _stdcall DetectorSetParam(Detector * detector, int DSize_W = 0, int DSize_H = 0, float AConfidenceThreshold = .0f, float ARefineThreshold = .0f);
extern "C" LIB_DETECTION long _stdcall DetectorGetObjectString(int AObj_idx, char* AObjName, int AObjSize);
