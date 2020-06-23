#include "dllheader.h"

#include <set>

Tracker::Tracker(int width, int height,
                          float sigma_l, float sigma_h ,
                          float sigma_iou, size_t t_min, size_t dist_min)
    : width(width), height(height),
    sigma_l(sigma_l), sigma_h(sigma_h),
    sigma_iou(sigma_iou), t_min(t_min), dist_min(dist_min),
    frame_count(0), track_count(0) {
};

void Tracker::update(std::vector<Box> detections) {
    float _iou;
    std::set<Park*> checked;

    // find hard-positive examples
    for (auto& park : parkings) {
        for (const auto& box : detections) {
            _iou = iou(box.x / static_cast<float>(width),
                       box.y / static_cast<float>(width),
                       box.x2 / static_cast<float>(width),
                       box.y2 / static_cast<float>(width),
                       park.x, park.y, park.x2, park.y2);

            if (_iou > iou_thresh) {
                if (park.state != Park::State::parking_invisible) {
                    park.state = Park::State::parking;
                }
                checked.insert(&park);
                break;
            }
        }
    }

    // update tracker
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

        if (checked.find(&park) != checked.end()) {
            continue;
        }

        for (const auto& track : track_active) {
            if (abs(track->boxes.back().y - track->boxes.front().y) > dist_min ||
                track->boxes.size() < t_min) {

                _iou = iou(track->boxes.back().x / static_cast<float>(width),
                    track->boxes.back().y / static_cast<float>(width),
                    track->boxes.back().x2 / static_cast<float>(width),
                    track->boxes.back().y2 / static_cast<float>(width),
                    park.x, park.y, park.x2, park.y2);

                if (max_iou < _iou) {
                    max_iou = _iou;

                    if (track->boxes.size() < t_min) {
                        max_state = Park::State::empty;
                    } else if (!((track->boxes.front().y < height / 2) ^
                        (track->boxes.back().y - track->boxes.front().y < 0))) {
                        // enter
                        max_state = Park::State::parking_invisible;
                    } else if (abs(track->boxes.back().y - track->boxes.front().y) > dist_min) {
                        // exit
                        max_state = Park::State::empty;
                    } else {
                        max_state = park.state;
                    }
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
        else {
            if (park.state == Park::State::parking) {
                park.state = Park::State::parking_invisible;
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
                                            float sigma_iou, size_t t_min, size_t dist_min) {
    AHandle->width = width;
    AHandle->height = height;

    AHandle->sigma_l = sigma_l;
    AHandle->sigma_h = sigma_h;
    AHandle->sigma_iou = sigma_iou;
    AHandle->t_min = t_min;
    AHandle->dist_min = dist_min;
    
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

LIB_TRACKING long _stdcall TrackerAddParking(Tracker * AHandle, float x, float y, float x2, float y2, Park::State state) {
    AHandle->parkings.push_back({ x, y, x2, y2, state});

    return 0;
}

LIB_TRACKING long _stdcall TrackerViewParks(Tracker* AHandle, const std::function<void(Park&)>& f) {
    for (auto& park : AHandle->parkings) {
        f(park);
    }

    return AHandle->parkings.size();
}

LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle) {
    return AHandle->parkings;
}

LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(Track&)>& f) {
    for (auto& track : AHandle->track_active) {
        f(*track);
    }

    return AHandle->track_active.size();
}

LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle) {
    return AHandle->track_active;
}