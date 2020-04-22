//
// Created by maybe on 4/22/2020.
//

#ifndef WARPING_LIBWARPING_H
#define WARPING_LIBWARPING_H
#include "libWarpingUtility.h"

class Calibration {
public:
    int w, h;
    float fx, fy, cx, cy, k[5];
    bool fisheye;
    const Size& size;

    Calibration(const Size& input_sz);
    Calibration(const Calibration& calib);

    void ConvertRays2Pixels(const Mat& rays, Mat* pixels_x, Mat* pixels_y) const;

    void Set(int w_, int h_, float fx_, float fy_, float cx_, float cy_, const float* k_, bool fisheye_);
    void Reset();
    void Clear();
};


class Warp {
public:
    int plane_mode;
    cv::Vec3f center, up, right;

    const Calibration& calib;
    const Size& output_size;

    Mat rays;
    Mat pix_x, pix_y;

    Warp(const Calibration& calib, const Size& output_sz, int plane_mode);

    void Update();
    void Map(const Mat& input, Mat& output) const;
    void DrawBoundingBox(Mat* output) const;

    void Reset();
    void Zoom(float dz = 1.f);
    void Rotate(float rot = .0f);
};

Calibration* CalibrationInit(const Size & input_sz);
Warp* WarpInit(const Calibration & calib, const Size& output_sz, int plane_mode = 2);

#endif //WARPING_LIBWARPING_H
