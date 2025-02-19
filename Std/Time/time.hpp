#pragma once
#include <std.hpp>
#include <chrono>
using Time_span = std::chrono::nanoseconds;
using Time_point = std::chrono::zoned_time<std::chrono::milliseconds>;
using Nano_time_point = std::chrono::time_point<std::chrono::steady_clock, Time_span>;
using Date = std::chrono::year_month_day;

constexpr const char* time_namespace{"std.time"};
struct Time_type_info {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "time_point";}
};
struct Time_span_type_info {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "time_span";}
};
struct Nano_time_type_info {
    static consteval const char* type_namespace() {return time_namespace;}
    static consteval const char* type_name() {return "nano_time_point";}
};

using T_time_span = Lazy_type<Time_span, Time_span_type_info>;
using T_time_point = Lazy_type<Time_point, Time_type_info>;
using T_nano_time_point = Lazy_type<Nano_time_point, Nano_time_type_info>;
