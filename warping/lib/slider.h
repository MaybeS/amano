//
// Created by 배지운 on 26/09/2019.
//

#ifndef AMANO_SLIDER_H
#define AMANO_SLIDER_H

#include <functional>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Slider {
    const String& window;
    const String name;
public:
    int value = 0;
    int value_max;

    Slider(const String& _window, const String& _name, int value_max, const function<void(int, void*)>& callback)
    : window(_window), name(_name) {
        cv::createTrackbar(name, window, &value, value_max, callback);
        
        createTrackbar( TrackbarName, "Display window", &alpha_slider, alpha_slider_max, on_trackbar );
        on_trackbar( alpha_slider, 0 );
        waitKey();
    }
};

#endif //AMANO_SLIDER_H
