#include "dllheader.h"

#include <set>

Tracker::Tracker(int width, int height,
                 float sigma_l, float sigma_h ,
                 float sigma_iou, int t_min,
                 float dist_min, int track_step)
    : width(width), height(height),
    sigma_l(sigma_l), sigma_h(sigma_h),
    sigma_iou(sigma_iou), t_min(t_min),
    dist_min(dist_min), track_step(track_step),
    frame_count(0), track_count(0) {
};

void Tracker::update(std::vector<Box>* detections) {
    float _iou;
    std::set<Park*> checked;

    // find hard-positive examples
    for (auto& park : parkings) {
        for (const auto& box : *detections) {
            _iou = iou(box.x / static_cast<float>(width),
                       box.y / static_cast<float>(width),
                       box.x2 / static_cast<float>(width),
                       box.y2 / static_cast<float>(width),
                       park.x, park.y, park.x2, park.y2);

            if (_iou > iou_thresh) {
                park.state = Park::State::parking;
                park.isExiting = false;
                park.isEntering = false;
                checked.insert(&park);
                break;
            }
        }
    }

    // update tracker
    for (Track* track : track_active) {
        const auto& src = track->boxes.back();

        if (!detections->empty()) {
            float max_iou = 0.f;
            size_t max_j = 0;
            for (size_t j = 0; j < detections->size(); j++) {
                const auto& tar = detections->at(j);

                _iou = iou(src.x, src.y, src.x2, src.y2,
                           tar.x, tar.y, tar.x2, tar.y2);
                if (max_iou < _iou) {
                    max_iou = _iou;
                    max_j = j;
                }
            }

            if (max_iou > sigma_iou) {
                track->boxes.emplace_back(detections->at(max_j));
                track->score = std::max(track->score, detections->at(max_j).prob);
                track_gc(track);

                track_update.emplace_back(track);
                detections->erase(detections->begin() + max_j);
            }
        }

        if ((track_update.empty() || !(track == track_update.back())) &&
            (track->score >= sigma_h && track->boxes.size() >= t_min)) {
            track_finish.emplace_back(track);
        }
    }

    for (const auto& detection : *detections) {
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
        const Track* max_track{ nullptr };

        if (checked.find(&park) != checked.end()) {
            continue;
        }

        for (const auto& track : track_active) {
            _iou = iou(track->boxes.back().x / static_cast<float>(width),
                track->boxes.back().y / static_cast<float>(width),
                track->boxes.back().x2 / static_cast<float>(width),
                track->boxes.back().y2 / static_cast<float>(width),
                park.x, park.y, park.x2, park.y2);

            if (max_iou < _iou) {
                max_iou = _iou;
                max_track = track;
            }
        }

        if (max_iou > iou_thresh) {
            // hard positive
            park.state = Park::State::parking;
        } else if (iou_min_thresh < max_iou) {
            // if pakring area cover track
            if (max_track != nullptr) {
                if (park.x <= max_track->boxes.back().x / static_cast<float>(width) && max_track->boxes.back().x2 / static_cast<float>(width) <= park.x2 &&
                    park.y <= max_track->boxes.back().y / static_cast<float>(width) && max_track->boxes.back().y2 / static_cast<float>(width) <= park.y2) {
                    park.state = Park::State::parking;
                    park.isExiting = false;
                    park.isEntering = false;
                } else if (isExiting(park, *max_track)) {
                    // weak positive
                    park.state = Park::State::empty;
                    park.isExiting = true;
                    park.isEntering = false;
                } else if (isEntering(park, *max_track)) {
                    park.state = Park::State::parking_invisible;
                    park.isExiting = false;
                    park.isEntering = true;
                } else {
                    park.state = park.state;
                }
            }
        } else {
            // not detected
            if (park.state == Park::State::parking) {
                park.state = Park::State::parking_invisible;
            }
        }
    }
}

void Tracker::finish() {
    for (auto& track : track_active) {
        if (track->score >= sigma_h && track->boxes.size() > t_min) {
            track_finish.emplace_back(track);
        }
    }
}

void Tracker::track_gc(Track* track) {
    if (track->boxes.size() < 300) {
        return;
    }
    track->boxes.erase(track->boxes.begin(), track->boxes.begin() + 250);
}

bool Tracker::assert_step(const Track& track) {
    return track.boxes.size() > track_step;
}

bool Tracker::isEntering(const Park& park, const Track& track) {
    if (!assert_step(track)) {
        return false;
    }

    std::vector<std::tuple<float, float>> points;
    size_t size = track.boxes.size();
    bool up_side = track.boxes.front().y < height / 2.f;
    float pcx = (park.x + park.x2) / 2.f, pcy = (park.y + park.y2) / 2.f;

    for (size_t index = 0; index + track_step < size; index += track_step) {
        float tx = 0.f, ty = 0.f;
        for (size_t j = index; j < index + track_step; j++) {
            tx += ((track.boxes[j].x + track.boxes[j].x2) * .5f) / static_cast<float>(width);
            ty += ((track.boxes[j].y + track.boxes[j].y2) * .5f) / static_cast<float>(width);
        }
        tx /= track_step;
        ty /= track_step;
        points.push_back({ tx / static_cast<float>(width),
                           ty / static_cast<float>(width) });
    }

    if (points.size() < 2) {
        return false;
    }

    bool flag = true;
    float lx = 0.f, ly = 0.f;

    for (const auto& point : points) {
        float bcx, bcy;
        std::tie(bcx, bcy) = point;

        if (up_side && (ly - bcy > -dist_min)) {
            // up side entering
        } else if (!up_side && (ly - bcy < dist_min )) {
            // down side entering
        } else if (abs(bcx - pcx) - abs(lx - pcx) < dist_min) {
            // x side entering
        } else {
            flag = false;
            break;
        }
        lx = bcx; ly = bcy;
    }
    return flag;
}
bool Tracker::isExiting(const Park& park, const Track& track) {
    if (!assert_step(track)) {
        return false;
    }

    std::vector<std::tuple<float, float>> points;
    size_t size = track.boxes.size();
    bool up_side = track.boxes.front().y < height / 2.f;
    float pcx = (park.x + park.x2) / 2.f, pcy = (park.y + park.y2) / 2.f;

    for (size_t index = 0; index + track_step < size; index += track_step) {
        float tx = 0.f, ty = 0.f;
        for (size_t j = index; j < index + track_step; j++) {
            tx += (track.boxes[j].x + track.boxes[j].x2) * .5f;
            ty += (track.boxes[j].y + track.boxes[j].y2) * .5f;
        }
        tx /= track_step;
        ty /= track_step;
        points.push_back({ tx / static_cast<float>(width), 
                           ty / static_cast<float>(width) });
    }

    if (points.size() < 2) {
        return false;
    }

    bool flag = true;
    float lx = 0.f, ly = 0.f;
    for (const auto& point : points) {
        float bcx, bcy;
        std::tie(bcx, bcy) = point;

        if (!lx && !ly) {
            lx = bcx; ly = bcy;
        }

        if (up_side && (ly - bcy < dist_min)) {
            // up side exiting
        } else if (!up_side && (ly - bcy > -dist_min)) {
            // down side exiting
        } else {
            flag = false;
            break;
        }
        lx = bcx; ly = bcy;
    }
    return flag;
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
                                            float sigma_iou, int t_min,
                                            float dist_min, int track_step) {
    AHandle->width = width;
    AHandle->height = height;

    AHandle->sigma_l = sigma_l;
    AHandle->sigma_h = sigma_h;
    AHandle->sigma_iou = sigma_iou;
    AHandle->t_min = t_min;
    AHandle->dist_min = dist_min;
    AHandle->track_step = track_step;
    
    return 0;
}

LIB_TRACKING long _stdcall TrackerUpdate(Tracker* AHandle, std::vector<Box>* boxes) {
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

    return static_cast<long>(AHandle->parkings.size());
}

LIB_TRACKING const std::vector<Park>& _stdcall TrackerParks(Tracker* AHandle) {
    return AHandle->parkings;
}

LIB_TRACKING long _stdcall TrackerViewTracks(Tracker* AHandle, const std::function<void(Track&)>& f) {
    for (auto& track : AHandle->track_active) {
        f(*track);
    }

    return static_cast<long>(AHandle->track_active.size());
}

LIB_TRACKING const std::vector<Track*>& _stdcall TrackerTracks(Tracker* AHandle) {
    return AHandle->track_active;
}