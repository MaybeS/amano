#pragma once

#include "LibDetection.h"


struct Track {
    size_t id;
    std::vector<Box> boxes;
    size_t start;
    float score;
};

class Tracker {
public:
    float sigma_l, sigma_h, sigma_iou;
    size_t t_min, frame_count, track_count;

    std::vector<Track*> track_active, track_update, track_finish;

    explicit Tracker(float sigma_l = 0.f, float sigma_h = .5f, float sigma_iou = .5f,
        size_t t_min = 25)
        : sigma_l(sigma_l), sigma_h(sigma_h), sigma_iou(sigma_iou),
        t_min(t_min), frame_count(0), track_count(0) {};

    void update(std::vector<Box> detections) {
        for (Track* track : track_active) {
            if (!detections.empty()) {
                size_t max_j = 0;
                float max_iou = 0.f;
                for (size_t j = 0; j < detections.size(); j++) {
                    float iou = this->iou(track->boxes.back(), detections[j]);
                    if (iou > max_iou) {
                        max_j = j;
                        max_iou = iou;
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
    }

    void finish() {
        for (auto track : track_active) {
            if (track->score >= sigma_h && track->boxes.size() > t_min) {
                track_finish.emplace_back(track);
            }
        }
    }

private:
    float iou(const Box& first, const Box& second) {
        float overlap_x0 = std::max(first.x, second.x);
        float overlap_y0 = std::max(first.y, second.y);
        float overlap_x1 = std::min(first.x2, second.x2);
        float overlap_y1 = std::min(first.y2, second.y2);

        if (overlap_x1 - overlap_x0 <= 0 || overlap_y1 - overlap_y0 <= 0) {
            return 0;
        }

        float size_first = (first.x2 - first.x) * (first.y2 - first.y);
        float size_second = (second.x2 - second.x) * (second.y2 - second.y);
        float size_intersection = (overlap_x1 - overlap_x0) * (overlap_y1 - overlap_y0);
        float size_union = size_first + size_second - size_intersection;

        return size_intersection / size_union;
    }
};
