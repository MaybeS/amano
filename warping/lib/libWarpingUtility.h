//
// Created by maybe on 4/22/2020.
//

#ifndef WARPING_LIBWARPINGUTILITY_H
#define WARPING_LIBWARPINGUTILITY_H
#include <opencv2/opencv.hpp>

using namespace cv;

template<typename T> inline T sqr(const T& v);
template <typename T> inline void Multiply3x3(const T* R1, const T* R2, T* R);
template <typename S, typename T> inline void RotateRay(S x, S y, S z, const T* R, T* ray);
template <typename T> inline void GetRotationMatrix(const T* rot, T* R);

inline Vec3f NormalizeVec3f(const Vec3f& v);

void MakeImageRays(const Size & sz, float fov, float h_dir, float v_dir, bool upright, const float* rot, Mat * rays, float y_ratio = 1.f);
void MakeCylinderAngleRays(const Size & sz, const Vec3f & center, const Vec3f & right, float deg, Mat * rays);
void MakeCylinderRays(const Size & sz, const Vec3f & center, const Vec3f & right, float deg, Mat * rays);
void MakePlaneRays(const Size & sz, const Vec3f & center, const Vec3f & up, const Vec3f & right, Mat * rays);
void GetBBoxRays(const Mat & rays, Mat * bbox_rays, int num_bbox_vertex = 20);

#endif //WARPING_LIBWARPINGUTILITY_H
