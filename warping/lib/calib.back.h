//
// Created by 배지운 on 25/09/2019.
//

#ifndef AMANO_CALIB_H
#define AMANO_CALIB_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Calibration {
public:
    int w, h;
    float fx, fy, cx, cy, k[5];
    bool fisheye;

    Calibration(const Size& input_sz) : w(0), h(0), fx(0), fy(0), cx(0), cy(0), k{0,}, fisheye(false) {
        this->w = input_sz.width; this->h = input_sz.height;
        // this->fx = this->fy = 0.55f * input_sz.height;
        this->fx = this->fy = 998.4f;
        // this->cx = input_sz.width / 2.f, this->cy = input_sz.height / 2.f;
        this->cx = 1997, this->cy = 1473;
        //ORG this->cx = input_sz.width / 2.f + 25, this->cy = input_sz.height / 2.f - 30;
        this->k[0] = -.25f; this->k[1] = -.0; this->k[2] = -.0; this->k[3] = -.0; this->k[4] = -.0;
        this->k[0] = 0.0715;
        this->k[1] = -0.0715;
        this->fisheye = true;
    }

    void Set(int w_, int h_, float fx_, float fy_, float cx_, float cy_, const float* k_, bool fisheye_) {
        w = w_, h = h_, fisheye = fisheye_;
        fx = fx_, fy = fy_, cx = cx_, cy = cy_;
        for (int i = 0; i < 5; ++i) {
            k[i] = (k_ == nullptr) ? 0 : k_[i];
        }
    }

    void Clear() {
        this->Set(0, 0, 0, 0, 0, 0, nullptr, false);
    }
};

#endif //AMANO_CALIB_H
