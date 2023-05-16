//
// Created by TianyaoChu on 2023/5/11.
//

#ifndef QUANTUM_NETWORKS_TIMING_H
#define QUANTUM_NETWORKS_TIMING_H

#include <chrono>
#include <string>

typedef std::chrono::steady_clock::time_point ClockTime;

ClockTime get_current_time();

std::string get_current_time_stamp();

int get_time_interval(ClockTime end_time, ClockTime beg_time);

double get_time_second(ClockTime end_time, ClockTime beg_time);

#endif //QUANTUM_NETWORKS_TIMING_H
