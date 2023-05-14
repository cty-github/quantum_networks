//
// Created by TianyaoChu on 2023/5/11.
//

#ifndef QUANTUM_NETWORKS_TIMING_H
#define QUANTUM_NETWORKS_TIMING_H

#include <chrono>

typedef std::chrono::steady_clock::time_point ClockTime;

ClockTime get_current_time();

int get_time_interval(ClockTime end_time, ClockTime beg_time);

#endif //QUANTUM_NETWORKS_TIMING_H
