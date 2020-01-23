#pragma once
//
// Created by น่ม๖ฟ๎ on 25/09/2019.
//

#ifndef AMANO_WARP_H
#define AMANO_WARP_H

#include <functional>

#include "utility.hpp"

using namespace std;

class Warp {
public:
	int plane_mode;
	Vec3f center, up, right;

	const Calibration& calib;
	const Size& output_size;

	Mat rays;
	Mat pix_x, pix_y;

	Warp(const Calibration& calib, const Size& output_size, int plane_mode = 2)
		: plane_mode(plane_mode), calib(calib), output_size(output_size) {
		Reset();
		Update();
	}

	void Update() {
		// update rays
		switch (plane_mode) {
			case 0: MakePlaneRays(output_size, center, up, right, &rays); break;
			case 1: MakeCylinderRays(output_size, center, right, 90, &rays); break;
			case 2: MakeCylinderAngleRays(output_size, center, right, 90, &rays); break;
		}
		ConvertRays2Pixels(calib, rays, &pix_x, &pix_y);
	}

	void Map(const Mat& input, Mat* output) const {
		remap(input, *output, pix_x, pix_y,
			INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	}

	void DrawBoundingBox(Mat* output) const {
		// draw outline to output
		Mat bbox_rays, pix_x, pix_y;
		GetBBoxRays(rays, &bbox_rays);
		ConvertRays2Pixels(calib, bbox_rays, &pix_x, &pix_y);

		const int line_width = 3;
		auto color = CV_RGB(255, 0, 0);
		for (int i = 0; i < bbox_rays.rows; ++i) {
			for (int j = 1; j < bbox_rays.cols; ++j) {
				const Point p0(pix_x.at<float>(i, j - 1), pix_y.at<float>(i, j - 1));
				const Point p1(pix_x.at<float>(i, j), pix_y.at<float>(i, j));
				line(*output, p0, p1, color, line_width);
			}
		}

		{
			auto color = CV_RGB(0, 255, 0);
			const Point center(calib.cx, calib.cy);
			line(*output, Point(center.x - 10, center.y), Point(center.x + 10, center.y), color, 2);
			line(*output, Point(center.x, center.y - 10), Point(center.x, center.y + 10), color, 2);

			const int num_horizon_rays = 36;
			Mat horizon_rays(1, num_horizon_rays + 1, CV_32FC3);
			for (int i = 0; i <= num_horizon_rays; ++i) {
				const double th = 2 * CV_PI * i / num_horizon_rays;
				float* ptr = (float*)horizon_rays.ptr(0, i);
				ptr[0] = cos(th), ptr[1] = sin(th), ptr[2] = 0;
			}
			ConvertRays2Pixels(calib, horizon_rays, &pix_x, &pix_y);
			for (int j = 1; j < horizon_rays.cols; ++j) {
				const Point p0(pix_x.at<float>(0, j - 1), pix_y.at<float>(0, j - 1));
				const Point p1(pix_x.at<float>(0, j), pix_y.at<float>(0, j));
				line(*output, p0, p1, color, line_width);
			}
		}
	}

	void Reset() {
		center << 0.0, 0.0, 1.0;
		up << 0.0, 1.0, 0.0;
		right << 1.0, 0.0, 0.0;
		right *= double(output_size.width) / output_size.height / 2.f;
	}

	void Zoom(float dz = 1.0f) {
		center = center * dz;
	}

	void Rotate(float rot = 0.0f) {
		Mat_<float> R(3, 3);
		R << cos(rot), -sin(rot), 0,
			sin(rot), cos(rot), 0,
			0, 0, 1;
		Mat rot_up = R * Mat(up);
		Mat rot_right = R * Mat(right);
		up = Vec3f((float*)rot_up.data);
		right = Vec3f((float*)rot_right.data);
		// Mat rotation = ( * Mat(_up);
		// up = Vec3f((float*) rotation.data);
	}
};

#endif //AMANO_WARP_H
