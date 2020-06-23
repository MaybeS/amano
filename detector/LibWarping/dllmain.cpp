#include "dllheader.h"


Calibration::Calibration(const Size& input_sz) : 
	size(input_sz), w(0), h(0), fx(0), fy(0), cx(0), cy(0), k{ 0, }, fisheye(false) {

	this->w = input_sz.width; this->h = input_sz.height;
	this->fx = this->fy = 0.34f * input_sz.height;
	this->cx = input_sz.width / 2.f, this->cy = input_sz.height / 2.f;
	this->k[0] = -.25f; this->k[1] = -.0; this->k[2] = -.0; this->k[3] = -.0; this->k[4] = -.0;
	this->k[0] = .0715f;
	this->k[1] = -.0715;
	this->fisheye = true;
}

Calibration::Calibration(const Calibration& calib) : size(calib.size) {
	Set(calib.w, calib.h, calib.fx, calib.fy, calib.cx, calib.cy, calib.k, calib.fisheye);
}


void Calibration::ConvertRays2Pixels(const Mat& rays, Mat* pixels_x, Mat* pixels_y) const {
	Mat& pix_x = *pixels_x;
	Mat& pix_y = *pixels_y;
	
	const float fx = this->fx, fy = this->fy, cx = this->cx, cy = this->cy;
	const float* k = this->k;
	const Size sz = rays.size();

	pix_x.create(sz, CV_32F);
	pix_y.create(sz, CV_32F);

	if (this->fisheye) {
		for (int y = 0; y < sz.height; ++y) {
			for (int x = 0; x < sz.width; ++x) {
				const float* ray = (const float*)rays.ptr(y, x);
				float rx = ray[0], ry = ray[1], rz = ray[2];
				float rn = sqrt(rx * rx + ry * ry + rz * rz) + 1e-12;
				rx /= rn, ry /= rn, rz /= rn;
				float rad = sqrt(rx * rx + ry * ry) + 1e-12;
				float th = atan2(rad, rz);
				th = th * (1 + th * (k[0] + th * k[1]));
				float px = rx * th / rad, py = ry * th / rad;
				pix_x.at<float>(y, x) = fx * px + cx;
				pix_y.at<float>(y, x) = fy * py + cy;
			}
		}
	} else {
		for (int y = 0; y < sz.height; ++y) {
			for (int x = 0; x < sz.width; ++x) {
				const float* ray = (const float*)rays.ptr(y, x);
				float rx = ray[0], ry = ray[1], rz = ray[2];
				rx /= rz, ry /= rz;
				float x2 = rx * rx, y2 = ry * ry, xy = rx * ry;
				float r2 = x2 + y2;
				float rc = 1 + r2 * (k[0] + r2 * (k[1] + r2 * k[4]));
				float tcx = k[2] * 2 * xy + k[3] * (3 * x2 + y2);
				float tcy = k[2] * (x2 + 3 * y2) + k[3] * 2 * xy;
				float px = rc * rx + tcx, py = rc * ry + tcy;
				pix_x.at<float>(y, x) = fx * px + cx;
				pix_y.at<float>(y, x) = fy * py + cy;
			}
		}
	}
}

void Calibration::Set(int w_, int h_, float fx_, float fy_, float cx_, float cy_, const float* k_, bool fisheye_) {
	w = w_, h = h_, fisheye = fisheye_;
	fx = fx_, fy = fy_, cx = cx_, cy = cy_;
	for (int i = 0; i < 5; ++i) {
		k[i] = (k_ == nullptr) ? 0 : k_[i];
	}
}

void Calibration::Reset() {
	this->w = size.width; this->h = size.height;
	this->fx = this->fy = 0.34f * size.height;
	this->cx = size.width / 2.f, this->cy = size.height / 2.f;
	this->k[0] = -.25f; this->k[1] = -.0; this->k[2] = -.0; this->k[3] = -.0; this->k[4] = -.0;
	this->k[0] = .0715f;
	this->k[1] = -.0715;
	this->fisheye = true;
}

void Calibration::Clear() {
	this->Set(0, 0, 0, 0, 0, 0, nullptr, false);
}



Warp::Warp(const Calibration& calib, const Size& output_sz, int plane_mode)
	: plane_mode(plane_mode), calib(calib), output_size(output_sz) {
	Reset();
	Update();
}

void Warp::Update() {
	// update rays
	switch (plane_mode) {
		case 0: MakePlaneRays(output_size, center, up, right, &rays); break;
		case 1: MakeCylinderRays(output_size, center, right, 90, &rays); break;
		case 2: MakeCylinderAngleRays(output_size, center, right, 90, &rays, angle); break;
	}
	this->calib.ConvertRays2Pixels(rays, &pix_x, &pix_y);
}

void Warp::Map(const Mat& input, Mat* output) const {
	remap(input, *output, pix_x, pix_y,
		INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
}

void Warp::DrawBoundingBox(Mat* output) const {
	// draw outline to output
	Mat bbox_rays, pix_x, pix_y;
	GetBBoxRays(rays, &bbox_rays);
	this->calib.ConvertRays2Pixels(bbox_rays, &pix_x, &pix_y);

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
		this->calib.ConvertRays2Pixels(horizon_rays, &pix_x, &pix_y);
		for (int j = 1; j < horizon_rays.cols; ++j) {
			const Point p0(pix_x.at<float>(0, j - 1), pix_y.at<float>(0, j - 1));
			const Point p1(pix_x.at<float>(0, j), pix_y.at<float>(0, j));
			line(*output, p0, p1, color, line_width);
		}
	}
}

void Warp::Reset() {
	center << .0f, .0f, 1.f;
	up << .0f, 1.f, .0f;
	right << 1.f, .0f, .0f;
	right *= double(output_size.width) / output_size.height / 2.f;
}

void Warp::Zoom(float dz) {
	center = center * dz;
}

void Warp::Rotate(float rot) {
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

LIB_WARPING Calibration* _stdcall CalibrationInit(const Size& input_sz) {
	return new Calibration(input_sz);
}

LIB_WARPING Warp* _stdcall WarpInit(const Calibration& calib, const Size& output_sz, int plane_mode) {
	return new Warp(calib, output_sz, plane_mode);
}