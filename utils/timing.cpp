//
// Created by TianyaoChu on 2023/5/11.
//

#include "timing.h"
#include <sstream>
#include <iomanip>

ClockTime get_current_time() {
    return std::chrono::steady_clock::now();
}

std::string get_current_time_stamp(){
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto local_time = std::localtime(&time_t);
    std::stringstream ss;
    ss << std::put_time(local_time, "%Y-%m-%d-%H-%M-%S") << std::setfill('0');
    std::string str;
    ss >> str;
    return str;
}

int get_time_interval(ClockTime end_time, ClockTime beg_time) {
    // in unit of 1/10000 second
    auto time_diff = end_time - beg_time;
    int time_interval = (int)std::chrono::duration_cast<std::chrono::microseconds>(time_diff).count()/100;
    return time_interval;
}

double get_time_second(ClockTime end_time, ClockTime beg_time) {
    // in unit of second
    auto time_diff = end_time - beg_time;
    double time_interval = (double)std::chrono::duration_cast<std::chrono::microseconds>(time_diff).count()/1000000;
    return time_interval;
}
