#pragma once
#include <algorithm>

template<typename T = float>
T iou(T p1x1, T p1y1, T p1x2, T p1y2,
    T p2x1, T p2y1, T p2x2, T p2y2) {
    T overlap_x0 = std::max(p1x1, p2x1), overlap_y0 = std::max(p1y1, p2y1);
    T overlap_x1 = std::min(p1x2, p2x2), overlap_y1 = std::min(p1y2, p2y2);

    if (overlap_x1 - overlap_x0 <= 0 || overlap_y1 - overlap_y0 <= 0) {
        return 0;
    }

    T size_first = (p1x2 - p1x1) * (p1y2 - p1y1);
    T size_second = (p2x2 - p2x1) * (p2y2 - p2y1);

    T size_intersection = (overlap_x1 - overlap_x0) * (overlap_y1 - overlap_y0);
    T size_union = size_first + size_second - size_intersection;

    return size_intersection / size_union;
}
