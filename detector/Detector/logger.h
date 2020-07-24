#pragma once
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <ctime>
#include <direct.h>

class Logger {
public:
	Logger()
	: detection_count(0), tracking_count(0), stamp(time(0)) {};
	Logger(const std::string& directory)
	: detection_count(0), tracking_count(0), parking_count(0), stamp(time(0)) {

		detection_prefix = directory + "//..//logs//" + std::to_string(stamp) + "-detection";
		tracking_prefix = directory + "//..//logs//" + std::to_string(stamp) + "-tracking";
		parking_prefix = directory + "//..//logs//" + std::to_string(stamp) + "-parking";

		_mkdir(std::string(directory + "//..//logs").c_str());
		_mkdir(detection_prefix.c_str());
		_mkdir(tracking_prefix.c_str());
		_mkdir(parking_prefix.c_str());
	}

	void write_detection(const std::function<void(std::ofstream& stream)>& f) {
		std::string ss(detection_prefix + "//" + std::to_string(detection_count) + ".txt");
		detection.open(ss.c_str());
		f(detection);
		detection.close();
		detection_count += 1;
	}

	void write_tracking(const std::function<void(std::ofstream& stream)>& f) {
		tracking.open(tracking_prefix + "//" + std::to_string(tracking_count) + ".txt");
		f(tracking);
		tracking.close();
		tracking_count += 1;
	}

	void write_parking(const std::function<void(std::ofstream& stream)>& f) {
		parking.open(parking_prefix + "//" + std::to_string(parking_count) + ".txt");
		f(parking);
		parking.close();
		parking_count += 1;
	}

private:
	size_t detection_count;
	size_t tracking_count;
	size_t parking_count;

	std::string detection_prefix;
	std::string tracking_prefix;
	std::string parking_prefix;

	std::ofstream detection, tracking, parking;
	time_t stamp;
};