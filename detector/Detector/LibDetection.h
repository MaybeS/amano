#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

#ifdef BUILD_DLL
	#define LIB_DETECTION __declspec(dllexport)
#else
	#define LIB_DETECTION __declspec(dllimport)
#endif

struct LIB_DETECTION Box {
	float x, y, x2, y2;
	float prob;
	unsigned int obj_id;
	unsigned int track_id;
	unsigned int frames_counter;
};

class LIB_DETECTION Detector;

extern "C" LIB_DETECTION bool _stdcall DetectorDevice();
extern "C" LIB_DETECTION Detector * _stdcall DetectorInit(const char* filename, bool cuda = false);
extern "C" LIB_DETECTION long _stdcall DetectorRelease(Detector * AHanlde);
extern "C" LIB_DETECTION int _stdcall DetectorClassify(Detector * AHandle, const cv::Mat & AImage);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetect(Detector * AHandle, cv::Mat & AImage, bool refine = true);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetectImage(Detector * AHanlde, const char* filename);
extern "C" LIB_DETECTION std::vector<Box> * _stdcall DetectorDetectMat(Detector * AHandle, unsigned char* pArray, int nArraySize);
extern "C" LIB_DETECTION long _stdcall DetectorSetParam(Detector * AHandle, int DSize_W = 0, int DSize_H = 0, float AConfidenceThreshold = .0f, float ARefineThreshold = .0f);
extern "C" LIB_DETECTION long _stdcall DetectorGetObjectString(int AObj_idx, char* AObjName, int AObjSize);
