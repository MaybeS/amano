#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <functional>
#include <assert.h>

#include "utility.h"

#ifdef BUILD_DLL
    #define LIB_TRACKING __declspec(dllexport)
#else
    #define LIB_TRACKING __declspec(dllimport)
#endif

struct Box {
    float x, y, x2, y2;
    float prob;
    unsigned int obj_id;
    unsigned int track_id;
    unsigned int frames_counter;
};

struct LIB_TRACKING Track {
    size_t id;
    std::vector<Box> boxes;
    size_t start;
    float score;
};

struct LIB_TRACKING Park {
    float x, y, x2, y2;
    enum class State {
        empty, parking, parking_invisible,
    } state;

public:
    bool isEmpty() {
        return this->state == State::empty;
    }
    bool isParking() {
        return this->state >= State::parking;
    }
    bool isVisible() {
        return this->state == State::parking;
    }
    bool isParkingInvisible() {
        return this->state == State::parking_invisible;
    }
};

class LIB_TRACKING Tracker {
public:
    int width, height;

    float sigma_l, sigma_h, sigma_iou;
    size_t t_min, frame_count, track_count;
    float iou_thresh = .5f;
    float iou_min_thresh = .1f;

    std::vector<Track*> track_active, track_update, track_finish;
    std::vector<Park> parkings;

    Tracker(int width, int height,
            float sigma_l = 0.f, float sigma_h = .5f,
            float sigma_iou = .5f, size_t t_min = 25);

    void update(std::vector<Box> detections);
    void finish();
};

extern "C" LIB_TRACKING Tracker * _stdcall TrackerInit(int width, int height);
extern "C" LIB_TRACKING long _stdcall TrackerRelease(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerSetParam(int width, int height,
                                                      float sigma_l = 0.f, float sigma_h = .5f,
                                                      float sigma_iou = .5f, size_t t_min = 25);

extern "C" LIB_TRACKING long _stdcall TrackerUpdate(Tracker* AHandle, const std::vector<Box>& boxes);
extern "C" LIB_TRACKING long _stdcall TrackerViewParks(Tracker* AHandle, const std::function<void(const Park&)>& f);
extern "C" LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(const Track&)> & f);
extern "C" LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle);
