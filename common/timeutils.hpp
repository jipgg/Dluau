#pragma once
#include <chrono>
namespace timeutils {
using namespace std::chrono;
inline system_clock::time_point local_time(const system_clock::time_point& tp) {
    auto local_zone = current_zone();
    auto local_time = zoned_time<system_clock::duration>(local_zone, tp);
    return local_time.get_sys_time();
}
}
