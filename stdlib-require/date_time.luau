export type datetime = typeof(setmetatable({} :: _datetime, {} :: __datetime))
type _datetime = {
    year: number,
    month: number,
    day: number,
    hour: number,
    minute: number,
    second: number,
}
type __datetime = {
    __sub: (self: datetime, other: datetime) -> duration,
    __add: (self: datetime, other: duration) -> duration,
}
export type duration = typeof(setmetatable({} :: _duration, {} :: __duration))
type _duration = {
    nanoseconds: number,
    milliseconds: number,
    seconds: number,
    minutes: number,
    hours: number,
}
type __duration = {
    __sub: (self: duration, other: duration) -> duration,
    __add: (self: duration, other: duration) -> duration,
}

export type date_time = {
    seconds: (s: number) -> duration,
    nanoseconds: (ns: number) -> duration,
    milliseconds: (ms: number) -> duration,
    microseconds: (us: number) -> duration,
    minutes: (min: number) -> duration,
    hours: (h: number) -> duration,
    local_now: () -> datetime,
    utc_now: () -> datetime,
}
return dllimport("lumin-stdlib", "luminstd_datetime")() :: date_time
