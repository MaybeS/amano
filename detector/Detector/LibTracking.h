#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <functional>
#include <assert.h>

#ifdef BUILD_DLL
    #define LIB_TRACKING __declspec(dllexport)
#else
    #define LIB_TRACKING __declspec(dllimport)
#endif

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

class LIB_TRACKING Tracker;

extern "C" LIB_TRACKING Tracker* _stdcall TrackerInit(int width, int height);
extern "C" LIB_TRACKING long _stdcall TrackerRelease(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerSetParam(Tracker* AHandle, int width, int height,
                                                      float sigma_l = 0.f, float sigma_h = .5f,
                                                      float sigma_iou = .5f, size_t t_min = 5, size_t dist_min = 30);

extern "C" LIB_TRACKING long _stdcall TrackerUpdate(Tracker* AHandle, const std::vector<Box>& boxes);

extern "C" LIB_TRACKING long _stdcall TrackerClearTracking(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerClearParking(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerSetParking(Tracker* AHandle, const std::vector<Park>& parks);
extern "C" LIB_TRACKING long _stdcall TrackerAddParking(Tracker* AHandle, float x, float y, float x2, float y2,
                                                        Park::State state = Park::State::empty);

extern "C" LIB_TRACKING long _stdcall TrackerViewParks(Tracker* AHandle, const std::function<void(Park&)>& f);
extern "C" LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle);
extern "C" LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(Track&)>& f);
extern "C" LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle);
