//
// Created by maybe on 4/22/2020.
//

#include "libWarpingUtility.h"

template <typename T> inline T sqr(const T& v) { return v * v; }

template <typename T> inline
void Multiply3x3(const T* R1, const T* R2, T* R) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            T v(0);
            for (int k = 0; k < 3; ++k) v += R1[i + 3 * k] * R2[k + 3 * j];
            R[i + 3 * j] = v;
        }
    }
}

template <typename S, typename T> inline
void RotateRay(S x, S y, S z, const T* R, T* ray) {
    ray[0] = R[0] * x + R[3] * y + R[6] * z,
    ray[1] = R[1] * x + R[4] * y + R[7] * z,
    ray[2] = R[2] * x + R[5] * y + R[8] * z;
}

inline Vec3f NormalizeVec3f(const Vec3f& v) {
    const double c = 1.0 / norm(v);
    return Vec3f(v[0] * c, v[1] * c, v[2] * c);
}

template <typename T> inline
void GetRotationMatrix(const T* rot, T* R) {
    const double th = sqrt(sqr(rot[0]) + sqr(rot[1]) + sqr(rot[2]));
    const double a = cos(th / 2), s = sin(th / 2) / (th + 1e-9);
    const double b = -rot[0] * s, c = -rot[1] * s, d = -rot[2] * s;
    const double aa = sqr(a), bb = sqr(b), cc = sqr(c), dd = sqr(d),
            ab = a * b, ac = a * c, ad = a * d, bc = b * c, bd = b * d, cd = c * d;
    R[0] = aa + bb - cc - dd, R[3] = 2 * (bc + ad), R[6] = 2 * (bd - ac),
    R[1] = 2 * (bc - ad), R[4] = aa + cc - bb - dd, R[7] = 2 * (cd + ab),
    R[2] = 2 * (bd + ac), R[5] = 2 * (cd - ab), R[8] = aa + dd - bb - cc;
}

void MakeImageRays(const Size& sz, float fov, float h_dir, float v_dir, bool upright, const float* rot, Mat* rays, float y_ratio) {
    const double d2r = CV_PI / 180;
    const float cx = sz.width / 2.f, cy = sz.height / 2.f;
    const float ff = .5f * sz.width / tan(fov * d2r / 2);
    const float h_rot[] = { 0.f, 0.f, static_cast<float>(h_dir * d2r) };
    const float v_rot[] = { static_cast<float>((90 - v_dir) * d2r), 0.f, 0.f };
    const float no_rot[] = { 0.f, 0.f, 0.f };
    rays->create(sz, CV_32FC3);

    float R_calib[9], R_h[9], R_v[9], R_dir[9], R[9];
    GetRotationMatrix(rot ? rot : no_rot, R_calib);
    GetRotationMatrix(h_rot, R_h);
    if (upright) {
        Multiply3x3(R_calib, R_h, R);
        const float y_offset = tan(v_dir * d2r);
        for (int y = 0; y < sz.height; ++y) {
            for (int x = 0; x < sz.width; ++x) {
                const float nx = (x - cx) / ff, ny = y_ratio * (y - cy) / ff + y_offset;
                RotateRay(nx, -1.f, ny, R, (float*)rays->ptr(y, x));
            }
        }
    }
    else {
        GetRotationMatrix(v_rot, R_v);
        Multiply3x3(R_h, R_v, R_dir);
        Multiply3x3(R_calib, R_dir, R);
        for (int y = 0; y < sz.height; ++y) {
            for (int x = 0; x < sz.width; ++x) {
                const float nx = (x - cx) / ff, ny = y_ratio * (y - cy) / ff;
                RotateRay(nx, ny, 1.f, R, (float*)rays->ptr(y, x));
            }
        }
    }
}

void MakeCylinderAngleRays(const Size& sz, const Vec3f& center, const Vec3f& right, float deg, Mat* rays) {
    const double d2r = CV_PI / 180;
    const int w = sz.width, h = sz.height;
    const Vec3f up = NormalizeVec3f(right.cross(center));
    const Vec3f front = NormalizeVec3f(up.cross(right));
    const double rad = center.dot(front);
    const double dx = 2.0 / (w - 1), dr = 2.0 / (h - 1);
    const float half_x = sz.width / 2.f;

    rays->create(sz, CV_32FC3);
    for (int y = 0; y < sz.height; ++y) {
        const double th = (y * dr - 1.0) * deg * d2r;
        const Vec3f ray_y = -sin(th) * up + cos(th) * front;
        for (int x = 0; x < sz.width; ++x) {
            double xx = tan((x - sz.width / 2.f) / (sz.width / 2.f) * (CV_PI / 3));

            const Vec3f r = NormalizeVec3f(ray_y + xx * right);
            memcpy(rays->ptr(y, x), r.val, sizeof(float) * 3);
        }
    }
}

void MakeCylinderRays(const Size& sz, const Vec3f& center, const Vec3f& right, float deg, Mat* rays) {
    const double d2r = CV_PI / 180;
    const int w = sz.width, h = sz.height;
    const Vec3f up = NormalizeVec3f(right.cross(center));
    const Vec3f front = NormalizeVec3f(up.cross(right));
    const double rad = center.dot(front);
    const double dx = 2.0 / (w - 1), dr = 2.0 / (h - 1);

    rays->create(sz, CV_32FC3);
    for (int y = 0; y < sz.height; ++y) {
        const double th = (y * dr - 1.0) * deg * d2r;
        const Vec3f ray_y = -sin(th) * up + cos(th) * front;
        for (int x = 0; x < sz.width; ++x) {
            const Vec3f r = NormalizeVec3f(ray_y + (x * dx - 1.0) * right);
            memcpy(rays->ptr(y, x), r.val, sizeof(float) * 3);
        }
    }
}

void MakePlaneRays(const Size& sz, const Vec3f& center, const Vec3f& up, const Vec3f& right, Mat* rays) {
    const int w = sz.width, h = sz.height;
    const double dx = 2.0 / (w - 1), dy = 2.0 / (h - 1);
    rays->create(sz, CV_32FC3);
    for (int y = 0; y < sz.height; ++y) {
        const Vec3f ray_y = center + (y * dy - 1.0) * up;
        for (int x = 0; x < sz.width; ++x) {
            const Vec3f r = NormalizeVec3f(ray_y + (x * dx - 1.0) * right);
            memcpy(rays->ptr(y, x), r.val, sizeof(float) * 3);
        }
    }
}

void GetBBoxRays(const Mat& rays, Mat* bbox_rays, int num_bbox_vertex) {
    const int w = rays.rows, h = rays.cols;
    num_bbox_vertex = std::min(num_bbox_vertex, std::min(w, h));
    bbox_rays->create(4, num_bbox_vertex + 1, CV_32FC3);
    const size_t elem_sz = sizeof(float) * 3;
    const double dx = (w - 1) / static_cast<double>(num_bbox_vertex);
    const double dy = (h - 1) / static_cast<double>(num_bbox_vertex);
    double x = 0, y = 0;
    for (int i = 0; i <= num_bbox_vertex; ++i) {
        const int ix = static_cast<int>(x), iy = static_cast<int>(y);
        memcpy(bbox_rays->ptr(0, i), rays.ptr(ix, 0), elem_sz);
        memcpy(bbox_rays->ptr(1, i), rays.ptr(0, iy), elem_sz);
        memcpy(bbox_rays->ptr(2, i), rays.ptr(ix, h - 1), elem_sz);
        memcpy(bbox_rays->ptr(3, i), rays.ptr(w - 1, iy), elem_sz);
        x += dx, y += dy;
    }
}
