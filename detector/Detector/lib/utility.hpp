#ifndef AMANO_DETECTOR_H
#define AMANO_DETECTOR_H

#include <opencv2/opencv.hpp>

#include "calib.hpp"

using namespace cv;

template <typename T> inline T sqr(const T& v);

template <typename T> inline
void Multiply3x3(const T* R1, const T* R2, T* R);

template <typename S, typename T> inline
void RotateRay(S x, S y, S z, const T* R, T* ray);

inline Vec3f NormalizeVec3f(const Vec3f& v);

template <typename T> inline
void GetRotationMatrix(const T* rot, T* R);

void ConvertRays2Pixels(const Calibration& calib, const Mat& rays,
	Mat* pixels_x_ptr, Mat* pixels_y_ptr);
void MakeImageRays(const Size& sz, float fov, float h_dir, float v_dir,
	bool upright, const float* rot, Mat* rays,
	float y_ratio = 1.f);

void MakeCylinderAngleRays(const Size& sz, const Vec3f& center, const Vec3f& right, float deg, Mat* rays);
void MakeCylinderRays(const Size& sz, const Vec3f& center, const Vec3f& right, float deg, Mat* rays);

void MakePlaneRays(const Size& sz, const Vec3f& center, const Vec3f& up, const Vec3f& right, Mat* rays);

void GetBBoxRays(const Mat& rays, Mat* bbox_rays, int num_bbox_vertex = 20);

#endif
