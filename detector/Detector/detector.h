#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "acognitive_post_def.h"

#ifdef DETECTOR_EXPORT
	#define DETECTOR_API __declspec(dllexport)
#else
	#define DETECTOR_API __declspec(dllimport)
#endif

struct MNBoxT {
	float x, y, w, h;
	float prob;
	unsigned int  obj_id;
	unsigned int  track_id;
	unsigned int  frames_counter;
};
class DETECTOR_API MNHandle;

extern "C" DETECTOR_API MNHandle * _stdcall MN_Init(const char* filename, int AGpu);
extern "C" DETECTOR_API long _stdcall MN_Dispose(MNHandle * AHanlde);
extern "C" DETECTOR_API int _stdcall MN_Classify(MNHandle * AHandle, const cv::Mat & AImage);
extern "C" DETECTOR_API std::vector<MNBoxT> * _stdcall MN_Detect(MNHandle * AHandle, cv::Mat & AImage);
extern "C" DETECTOR_API std::vector<MNBoxT> * _stdcall MN_detect_Image(MNHandle * AHanlde, const char* filename);
extern "C" DETECTOR_API std::vector<MNBoxT> * _stdcall MN_detect_Mat(MNHandle * AHandle, unsigned char* pArray, int nArraySize);
extern "C" DETECTOR_API long _stdcall MN_SetParam(MNHandle * AHandle, int DSize_W = 0, int DSize_H = 0, float AConfidenceThreshold = .0f, float ARefineThreshold = .0f);
extern "C" DETECTOR_API long _stdcall MN_GetObjectString(int AObj_idx, char* AObjName, int AObjSize);
