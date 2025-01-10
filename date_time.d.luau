export type duration_units = 'ns'|'us'|'ms'|'s'|'min'|'h'|'d'|'mo'|'y'
declare class duration
    in_nanoseconds: number
    in_milliseconds: number
    in_seconds: number
    in_minutes: number
    in_hours: number
    function __sub(self, other: duration): duration
    function __call(self, d: duration_units): duration
    function __add(self, other: duration): duration
end
declare class time_point
    year: number
    month: number
    hour: number
    minute: number
    second: number
    function __sub(self, other: time_point): duration
    function __add(self, other: time_point): duration
    function __concat(self, fmt: string): string
    function __mod(self, fmt: string): string
end
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
    zoned_time: (tp: time_point, time_zone: string) -> time_point,
    system_clock: clock_variant,
    steady_clock: clock_variant,
    gps_clock: clock_variant,
    rai_clock: clock_variant,
    high_resolution_clock: clock_variant,
    utc_clock: clock_variant,
}
