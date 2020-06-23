#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;

#ifdef BUILD_DLL
	#define LIB_WARPING __declspec(dllexport)
#else
	#define LIB_WARPING __declspec(dllimport)
#endif

template<typename T> LIB_WARPING inline T sqr(const T& v);
template <typename T> LIB_WARPING inline void Multiply3x3(const T* R1, const T* R2, T* R);
template <typename S, typename T> LIB_WARPING inline void RotateRay(S x, S y, S z, const T* R, T* ray);
template <typename T> LIB_WARPING inline void GetRotationMatrix(const T* rot, T* R);

LIB_WARPING inline Vec3f NormalizeVec3f(const Vec3f& v);

extern "C" LIB_WARPING void MakeImageRays(const Size & sz, float fov, float h_dir, float v_dir, bool upright, const float* rot, Mat * rays, float y_ratio = 1.f);
extern "C" LIB_WARPING void MakeCylinderAngleRays(const Size & sz, const Vec3f & center, const Vec3f & right, float deg, Mat * rays, float angle = 3.f);
extern "C" LIB_WARPING void MakeCylinderRays(const Size & sz, const Vec3f & center, const Vec3f & right, float deg, Mat * rays);
extern "C" LIB_WARPING void MakePlaneRays(const Size & sz, const Vec3f & center, const Vec3f & up, const Vec3f & right, Mat * rays);
extern "C" LIB_WARPING void GetBBoxRays(const Mat & rays, Mat * bbox_rays, int num_bbox_vertex = 20);
