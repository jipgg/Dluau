export type unit = 'ns'|'us'|'ms'|'s'|'min'|'h'|'d'|'mo'|'y'
export type duration = typeof(setmetatable({}::{
    in_nanoseconds: number,
    in_milliseconds: number,
    in_seconds: number,
    in_minutes: number,
    in_hours: number,
    format: (self: time_point, fmt: string) -> string,
},{}::{
    __sub: (self: duration, other: duration) -> duration,
    __add: (self: duration, other: duration) -> duration,
}))
export type time_point = typeof(setmetatable({}::{
    year: number,
    month: number,
    hour: number,
    minute: number,
    second: number,
    format: (self: time_point, fmt: string) -> string,
},{}::{
    __sub: (self: time_point, other: time_point | duration) -> duration,
    __add: (self: time_point, other: time_point | duration) -> duration,
}))
export type clock_variant = {
    now: () -> time_point,
}
export type module_date_time = {
    time_now: () -> time_point,
    seconds: (s: number) -> duration,
    nanoseconds: (ns: number) -> duration,
    milliseconds: (ms: number) -> duration,
    microseconds: (us: number) -> duration,
    minutes: (min: number) -> duration,
    days: (d: number) -> duration,
    months: (d: number) -> duration,
    hours: (h: number) -> duration,
    --zoned_time: (tp: time_point, time_zone: string) -> time_point,
    system_clock: clock_variant,
    steady_clock: clock_variant,
    gps_clock: clock_variant,
    rai_clock: clock_variant,
    high_resolution_clock: clock_variant,
    utc_clock: clock_variant,
}
return dllimport("lumin-stdlib", "module_date_time")() :: module_date_time
