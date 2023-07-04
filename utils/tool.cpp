//
// Created by TianyaoChu on 2023/4/13.
//

#include "tool.h"
#include <cmath>

double dist(double x1, double y1, double x2, double y2) {
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

bool ascend_compare_value(const std::pair<int, int>& pair1, const std::pair<int, int>& pair2) {
    return pair1.second < pair2.second;
}

bool descend_compare_value(const std::pair<int, int>& pair1, const std::pair<int, int>& pair2) {
    return pair1.second > pair2.second;
}
