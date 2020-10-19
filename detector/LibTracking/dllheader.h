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
    #define DLLEXPORT_STRUCT
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

DLLEXPORT_STRUCT struct LIB_TRACKING Track {
    int id;
    int start;
    float score;
    std::vector<Box> boxes;

    Track()
    : id(0), start(0), score(0.f) {}
    Track(int id, const std::vector<Box>& boxes, int start, float score)
    : id(id), boxes(boxes), start(start), score(score) {}
};

DLLEXPORT_STRUCT struct LIB_TRACKING Park {
    float x, y, x2, y2;
    enum class State {
        empty, parking, parking_invisible,
    } state;
    bool isEntering, isExiting;

public:
    bool isEmpty() const {
        return this->state == State::empty;
    }
    bool isExist() const {
        return this->state >= State::parking;
    }
    bool isVisible() const {
        return this->state == State::parking;
    }
    bool isInvisible() const {
        return this->state == State::parking_invisible;
    }
};

DLLEXPORT_STRUCT class LIB_TRACKING Tracker {
public:
    int width, height;

    float sigma_l, sigma_h, sigma_iou;
    int t_min, frame_count, track_count;
    float dist_min;
    float iou_thresh = .45f;
    float iou_min_thresh = .04f;
    int track_step;
    std::vector<Track*> track_active, track_update, track_finish;
    std::vector<Park> parkings;

    Tracker(int width, int height,
            float sigma_l = 0.f, float sigma_h = .5f,
            float sigma_iou = .5f, int t_min = 5,
            float dist_min = .01f, int track_step = 5);

    void update(std::vector<Box>* detections);
    void finish();

private:
    void track_gc(Track* track);
    bool assert_step(const Track& track);

    bool isEntering(const Park& park, const Track& track);
    bool isExiting(const Park& park, const Track& track);
};

extern "C" LIB_TRACKING Tracker* _stdcall TrackerInit(int width, int height);
extern "C" LIB_TRACKING long _stdcall TrackerRelease(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerSetParam(Tracker* AHandle, int width, int height,
                                                      float sigma_l = 0.f, float sigma_h = .5f,
                                                      float sigma_iou = .5f, int t_min = 5,
                                                      float dist_min = .005f, int track_step = 5);

extern "C" LIB_TRACKING long _stdcall TrackerUpdate(Tracker* AHandle, std::vector<Box>* boxes);

extern "C" LIB_TRACKING long _stdcall TrackerClearTracking(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerClearParking(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerSetParking(Tracker* AHandle, const std::vector<Park>& parks);
extern "C" LIB_TRACKING long _stdcall TrackerAddParking(Tracker* AHandle, float x, float y, float x2, float y2,
                                                        Park::State state = Park::State::empty);

extern "C" LIB_TRACKING long _stdcall TrackerViewParks(Tracker* AHandle, const std::function<void(Park&)>& f);
extern "C" LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(Track&)>& f);
extern "C" LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle);
