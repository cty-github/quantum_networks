//
// Created by TianyaoChu on 2023/5/11.
//

#include "timing.h"

ClockTime get_current_time() {
    return std::chrono::steady_clock::now();
}

int get_time_interval(ClockTime end_time, ClockTime beg_time) {
    // in unit of 1/10000 second
    auto time_diff = end_time - beg_time;
    auto time_interval = std::chrono::duration_cast<std::chrono::microseconds>(time_diff).count()/100;
    return (int)time_interval;
}
