#pragma once
#include <chrono>
#include <LazyUserdataType.hpp>
using TimeSpan = std::chrono::nanoseconds;
using Time = std::chrono::zoned_time<std::chrono::milliseconds>;
using NanoTime = std::chrono::time_point<std::chrono::steady_clock, TimeSpan>;
using Date = std::chrono::year_month_day;

constexpr const char* time_namespace{"gpm.time"};
struct TimeTypeInfo {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "Time";}
};
struct TimeSpanTypeInfo {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "TimeSpan";}
};
struct NanoTimeTypeInfo {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "NanoTime";}
};

using TimeSpanType = LazyUserdataType<TimeSpan, TimeSpanTypeInfo>;
using TimeType = LazyUserdataType<Time, TimeTypeInfo>;
using NanoTimeType = LazyUserdataType<NanoTime, NanoTimeTypeInfo>;
