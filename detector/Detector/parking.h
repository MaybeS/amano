#pragma once

#include "LibDetection.h"

class Parking {
public:
    float iou_thresh = .5f;
    float iou_min_thresh = .1f;
	std::vector<bool> is_parking;
    std::vector<std::tuple<float, float, float, float>> parkings;

    Parking(const std::vector<std::tuple<float, float, float, float>>& parkings)
    : parkings(parkings), is_parking(parkings.size(), FALSE) {

    }
    Parking(const std::vector<std::tuple<float, float, float, float>>& parkings,
        const std::vector<bool>& is_parking)
    : parkings(parkings), is_parking(is_parking) {

    }
	Parking(const std::vector<std::tuple<float, float, float, float>>& parkings,
            const std::vector<Box>& boxes)
    : parkings(parkings), is_parking(parkings.size(), FALSE) {
        for (size_t i = 0; i < parkings.size(); i++) {
            float x, y, x2, y2;
            std::tie(x, y, x2, y2) = parkings[i];

            for (const auto& box : boxes) {
                if (iou(box, Box{ x, y, x2, y2, 0, 0, 0, 0 }) > iou_thresh) {
                    is_parking[i] = TRUE;
                    break;
                }
            }
        }
	}

    void update(const std::vector<Box>& boxes, const std::vector<bool>& enters) {
        float iou_value = 0.f;
        float x, y, x2, y2;
        for (size_t i = 0; i < parkings.size(); i++) {
            std::tie(x, y, x2, y2) = parkings[i];
            Box park{x, y, x2, y2, 0, 0, 0, 0};

            float max_iou = 0.f;
            size_t max_j = 0;
            for (size_t j = 0; j < boxes.size(); j++) {
                const auto box = boxes[j];
                const auto enter = enters[j];

                iou_value = iou(box, park);
                if (iou_value > max_iou) {
                    max_iou = iou_value;
                    max_j = j;
                }
            }

            if (max_iou > iou_thresh) {
                is_parking[i] = TRUE;
            } else if (0 < max_iou && max_iou < iou_min_thresh) {
                is_parking[i] = enters[max_j];
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