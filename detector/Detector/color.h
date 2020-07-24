#pragma once

#include <opencv2/opencv.hpp>
#include "LibTracking.h"

class Color {
public:
	enum class Value {
		empty, parking, parking_invisible,
	} value;

	Color() = default;
	constexpr Color(Value value)
		: value(value) {}
	constexpr Color(Park::State state)
		: value(static_cast<Value>(static_cast<int>(state))) {}

	operator Value() const { return value; }
	explicit operator bool() = delete;

	cv::Scalar color() {
		switch (value) {
			case Value::empty:
				return cv::Scalar{ 0, 0, 255 };
			case Value::parking:
				return cv::Scalar{ 0, 255, 0 };
			case Value::parking_invisible:
				return cv::Scalar{ 0, 255, 255 };
		}
	}

	static cv::Scalar map(int seed = 0) {
		size_t MAP_SIZE = 210;
		size_t MAP[] = {
			59,76,192,60,78,194,61,80,195,62,81,197,63,83,
			198,64,85,200,66,87,201,67,88,203,68,90,204,69,
			92,206,70,93,207,71,95,209,73,97,210,74,99,211,
			75,100,213,76,102,214,77,104,215,79,105,217,
			80,107,218,81,109,219,82,110,221,84,112,222,
			85,114,223,86,115,224,87,117,225,89,119,226,
			90,120,228,91,122,229,93,123,230,94,125,231,
			95,127,232,96,128,233,98,130,234,99,131,235,
			100,133,236,102,135,237,103,136,238,104,138,239,
			106,139,239,107,141,240,108,142,241,110,144,242,
			111,145,243,112,147,243,114,148,244,115,150,245,
			116,151,246,118,153,246,119,154,247,120,156,247,
			122,157,248,123,158,249,124,160,249,126,161,250,
			127,163,250,129,164,251,130,165,251,131,167,252,
			133,168,252,134,169,252,135,171,253,137,172,253,
			138,173,253,140,174,254,141,176,254,142,177,254,
			144,178,254,145,179,254,147,181,255,148,182,255,
		};

		return cv::Scalar(
			MAP[seed % MAP_SIZE],
			MAP[(seed + 1) % MAP_SIZE],
			MAP[(seed + 2) % MAP_SIZE]
		);
	}
};

