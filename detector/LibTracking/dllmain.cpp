#include "dllheader.h"

Tracker::Tracker(int width, int height,
                          float sigma_l, float sigma_h ,
                          float sigma_iou, size_t t_min)
    : width(width), height(height),
    sigma_l(sigma_l), sigma_h(sigma_h),
    sigma_iou(sigma_iou), t_min(t_min),
    frame_count(0), track_count(0) {
};

void Tracker::update(std::vector<Box> detections) {
    // update tracker
    float _iou;

    for (Track* track : track_active) {
        const auto& src = track->boxes.back();

        if (!detections.empty()) {
            float max_iou = 0.f;
            size_t max_j = 0;
            for (size_t j = 0; j < detections.size(); j++) {
                const auto& tar = detections[j];

                _iou = iou(src.x, src.y, src.x2, src.y2,
                    tar.x, tar.y, tar.x2, tar.y2);
                if (max_iou < _iou) {
                    max_iou = _iou;
                    max_j = j;
                }
            }

            if (max_iou > sigma_iou) {
                track->boxes.emplace_back(detections[max_j]);
                track->score = std::max(track->score, detections[max_j].prob);

                track_update.emplace_back(track);
                detections.erase(detections.begin() + max_j);
            }
        }

        if ((track_update.empty() || !(track == track_update.back())) &&
            (track->score >= sigma_h && track->boxes.size() >= t_min)) {
            track_finish.emplace_back(track);
        }
    }

    for (const auto& detection : detections) {
        track_update.push_back(new Track{
            track_count++, {detection}, frame_count, detection.prob,
            });
    }

    track_active.swap(track_update);
    track_update.clear();

    frame_count++;

    // update parking check
    for (auto& park : parkings) {
        float max_iou = 0.f;
        Park::State max_state;

        for (const auto& track : track_active) {
            if (abs(track->boxes.back().y - track->boxes.front().y) > 10 ||
                track->boxes.size() < 10) {

                _iou = iou(track->boxes.back().x / width,
                    track->boxes.back().y / width,
                    track->boxes.back().x2 / width,
                    track->boxes.back().y2 / width,
                    park.x, park.y, park.x2, park.y2);

                if (max_iou < _iou) {
                    max_iou = _iou;

                    if (!((track->boxes.front().y < height / 2) ^
                        (track->boxes.back().y - track->boxes.front().y < 0))) {
                        // enter
                        max_state = Park::State::parking_invisible;
                    }
                    else if (abs(track->boxes.back().y - track->boxes.front().y) > 10) {
                        // exit
                        max_state = Park::State::empty;
                    }
                    else {
                        max_state = park.state;
                    }
                }
            }

            if (max_iou > iou_thresh) {
                // hard positive
                park.state = Park::State::parking;
            }
            else if (0 < max_iou && max_iou < iou_min_thresh) {
                // weak positive
                park.state = max_state;
            }
        }
    }
}

void Tracker::finish() {
    for (auto track : track_active) {
        if (track->score >= sigma_h && track->boxes.size() > t_min) {
            track_finish.emplace_back(track);
        }
    }
}

LIB_TRACKING Tracker* _stdcall TrackerInit(int width, int height) {
    return new Tracker(width, height);
}

LIB_TRACKING long _stdcall TrackerRelease(Tracker* AHandle) {
    for (auto& track : AHandle->track_active) {
        delete track;
    }
    for (auto& track : AHandle->track_update) {
        delete track;
    }
    for (auto& track : AHandle->track_finish) {
        delete track;
    }
    delete AHandle;
    return 0;
}

LIB_TRACKING  long _stdcall TrackerSetParam(Tracker* AHandle,
                                            int width, int height,
                                            float sigma_l, float sigma_h, 
                                            float sigma_iou, size_t t_min) {
    AHandle->width = width;
    AHandle->height = height;

    AHandle->sigma_l = sigma_l;
    AHandle->sigma_h = sigma_h;
    AHandle->sigma_iou = sigma_iou;
    AHandle->t_min = t_min;
    
    return 0;
}

LIB_TRACKING long _stdcall TrackerUpdate(Tracker* AHandle, const std::vector<Box>& boxes) {
    AHandle->update(boxes);

    return 0;
}

LIB_TRACKING long _stdcall TrackerClearTracking(Tracker* AHandle) {
    AHandle->track_active.clear();
    AHandle->track_finish.clear();
    AHandle->track_active.clear();

    return 0;
}

LIB_TRACKING long _stdcall TrackerClearParking(Tracker* AHandle) {
    AHandle->parkings.clear();;

    return 0;
}

LIB_TRACKING long _stdcall TrackerSetParking(Tracker * AHandle, const std::vector<Park>& parks) {
    TrackerClearParking(AHandle);
    AHandle->parkings = std::vector<Park>{ parks };

    return 0;
}

LIB_TRACKING long _stdcall TrackerAddParking(Tracker * AHandle, float x, float y, float x2, float y2) {
    AHandle->parkings.push_back({ x, y, x2, y2 });

    return 0;
}

LIB_TRACKING long _stdcall TrackerViewParks(Tracker* AHandle, const std::function<void(const Park&)>& f) {
    for (const auto& park : AHandle->parkings) {
        f(park);
    }

    return AHandle->parkings.size();
}

LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle) {
    return AHandle->parkings;
}

LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(const Track&)>& f) {
    for (const auto& track : AHandle->track_active) {
        f(*track);
    }

    return AHandle->track_active.size();
}

LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle) {
    return AHandle->track_active;
}