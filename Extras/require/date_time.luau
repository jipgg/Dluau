type chrono_literals = 'ns'|'us'|'s'|'min'|'h'|'d'|'mo'|'y' 
--- Represents a span of time, expressed in various units.
--- A `duration` is a userdata type that supports arithmetic operations and conversions between different time units.
export type duration = typeof(setmetatable({} :: _duration, {} :: __duration))
type _duration = {
    --- The duration represented in nanoseconds.
    in_nanoseconds: number,
    --- The duration represented in milliseconds.
    in_milliseconds: number,
    --- The duration represented in seconds.
    in_seconds: number,
    --- The duration represented in minutes.
    in_minutes: number,
    --- The duration represented in hours.
    in_hours: number,
    --- Formats the `duration` as a string according to the provided format string.
    --- Uses standard C++ formatting specifiers for std::chrono's `duration<nanoseconds>`.
    --- **Format specifiers**
    --- - `%H`: Hours as a two-digit number (e.g., `01` for 1 hour).
    --- - `%M`: Minutes as a two-digit number (e.g., `59`).
    --- - `%S`: Seconds with optional fractional seconds (e.g., `59.123456`).
    --- - `%T`: Full time representation, including hours, minutes, and seconds (e.g., `01:59:59.123`).
    --- - `%R`: Time without seconds (e.g., `01:59`).
    --- - `%p`: AM/PM designation for 12-hour clocks (e.g., `AM` or `PM`).
    --- **Example usage**
    --- ```lua
    --- local dt = require('@std/date_time')
    --- local duration = dt.hours(10) + dt.seconds(20)
    --- print(duration:format'%T')
    --- ```
    --- @param fmt string The format string to use for formatting.
    --- @return string The formatted string representation of the duration.
    format: ((self: duration, fmt: string ) -> string)
        & ((self: duration, presets:'%H'|'%M'|'%S'|'%T'|'%S'|'%R'|'%p') -> string),
    type: (self: duration) -> "date_time.duration",
}
type __duration = {
    __sub: (self: duration, other: duration) -> duration,
    __add: (self: duration, other: duration) -> duration,
    __type: "date_time.duration"
}
--- Represents a specific point in time, defined in terms of calendar date and time.
--- A `time_point` is a userdata type that supports formatting and arithmetic with durations.
export type time = typeof(setmetatable({} :: _time, {} :: __time))
type _time = {
    --- The year of the time point (e.g., 2025).
    year: number,
    --- The month of the time point (1-12).
    month: number,
    --- The day of the time point (1-31, depending on the month).
    day: number,
    --- The hour of the time point (0-23).
    hour: number,
    --- The minute of the time point (0-59).
    minute: number,
    --- The second of the time point (0-59).
    second: number,
    --- The millisecond of the time point (0-59).
    millisecond: number,
    --- Formats the `time_point` as a string according to the provided format string.
    --- Uses standard C++ formatting for std::chrono's `time_point<system_clock, nanoseconds>`.
    --- **Format specifiers**
    --- - `%Y`: Full year (e.g., `2025`).
    --- - `%C`: Century (year divided by 100, truncated, e.g., `20` for 2025).
    --- - `%y`: Last two digits of the year (e.g., `25` for 2025).
    --- - `%m`: Month as a two-digit number (e.g., `01` for January).
    --- - `%B`: Full month name (e.g., `January`).
    --- - `%b`: Abbreviated month name (e.g., `Jan`).
    --- - `%d`: Day of the month as a two-digit number (e.g., `05`).
    --- - `%e`: Day of the month, single-digit space-padded (e.g., ` 5` for 5th day).
    --- - `%H`: Hour in 24-hour format (e.g., `14` for 2 PM).
    --- - `%I`: Hour in 12-hour format (e.g., `02` for 2 PM).
    --- - `%M`: Minute as a two-digit number (e.g., `35`).
    --- - `%S`: Seconds with optional fractional seconds (e.g., `45.6789`).
    --- - `%p`: AM/PM designation (e.g., `PM`).
    --- - `%z`: UTC offset in `+HHMM` or `-HHMM` format (e.g., `+0530`).
    --- - `%Z`: Time zone abbreviation (e.g., `UTC`).
    --- - `%j`: Day of the year (e.g., `005` for January 5).
    --- - `%U`: Week number with Sunday as the first day (e.g., `01`).
    --- - `%W`: Week number with Monday as the first day (e.g., `01`).
    --- - `%c`: Locale-specific date and time (e.g., `Tue Jan 5 14:35:45 2025`).
    --- - `%x`: Locale-specific date representation (e.g., `01/05/2025`).
    --- - `%X`: Locale-specific time representation (e.g., `14:35:45`).
    --- - `%%`: Literal `%` character.
    --- **Example usage**
    --- ```lua
    --- local date_time = require('@std/date_time')
    --- local tp = date_time.now()
    --- print(tp:format'%Y-%m-%d %H:%M:%S')
    --- ```
    --- @param fmt string The format string to use for formatting.
    --- @return string The formatted string representation of the time point.
    format: ((self: time, fmt: string) -> string)
        & ((self: time, presets:'%Y'|'%C'|'%y'|'%m'|
            '%B'|'%b'|'%d'|'%e'|'%H'|'%I'|'%M'|'%S'|'%p'|
            '%z'|'%Z'|'%j'|'%U'|'%W'|'%c'|'%x'|'%X'|'%%') -> string),
    type: (self: time) -> "date_time.time",
}
type __time = {
    __sub: ((self: time, other: time) -> duration)
        & ((self: time, other: duration) -> time),
    __add: ((self: time, other: time) -> duration)
        & ((self: time, other: duration) -> time),
    __type: "date_time.time"
}
export type high_precision_time = typeof(setmetatable({} :: _high_precision_time, {} :: __high_precision_time))
type _high_precision_time = {
    hour: number,
    --- The minute of the time point (0-59).
    minute: number,
    --- The second of the time point (0-59).
    second: number,
    --- The millisecond of the time point.
    millisecond: number,
    --- The microsecond of the time point.
    microsecond: number,
    --- The nanosecond of the time point.
    nanosecond: number,
    --- Formats the `time_point` as a string according to the provided format string.
    --- Uses standard C++ formatting for std::chrono's `time_point<system_clock, nanoseconds>`.
    --- **Format specifiers**
    --- - `%Y`: Full year (e.g., `2025`).
    --- - `%C`: Century (year divided by 100, truncated, e.g., `20` for 2025).
    --- - `%y`: Last two digits of the year (e.g., `25` for 2025).
    --- - `%m`: Month as a two-digit number (e.g., `01` for January).
    --- - `%B`: Full month name (e.g., `January`).
    --- - `%b`: Abbreviated month name (e.g., `Jan`).
    --- - `%d`: Day of the month as a two-digit number (e.g., `05`).
    --- - `%e`: Day of the month, single-digit space-padded (e.g., ` 5` for 5th day).
    --- - `%H`: Hour in 24-hour format (e.g., `14` for 2 PM).
    --- - `%I`: Hour in 12-hour format (e.g., `02` for 2 PM).
    --- - `%M`: Minute as a two-digit number (e.g., `35`).
    --- - `%S`: Seconds with optional fractional seconds (e.g., `45.6789`).
    --- - `%p`: AM/PM designation (e.g., `PM`).
    --- - `%z`: UTC offset in `+HHMM` or `-HHMM` format (e.g., `+0530`).
    --- - `%Z`: Time zone abbreviation (e.g., `UTC`).
    --- - `%j`: Day of the year (e.g., `005` for January 5).
    --- - `%U`: Week number with Sunday as the first day (e.g., `01`).
    --- - `%W`: Week number with Monday as the first day (e.g., `01`).
    --- - `%c`: Locale-specific date and time (e.g., `Tue Jan 5 14:35:45 2025`).
    --- - `%x`: Locale-specific date representation (e.g., `01/05/2025`).
    --- - `%X`: Locale-specific time representation (e.g., `14:35:45`).
    --- - `%%`: Literal `%` character.
    --- **Example usage**
    --- ```lua
    --- local date_time = require('@std/date_time')
    --- local tp = date_time.now()
    --- print(tp:format'%Y-%m-%d %H:%M:%S')
    --- ```
    --- @param fmt string The format string to use for formatting.
    --- @return string The formatted string representation of the time point.
    format: ((self: high_precision_time, fmt: string) -> string)
        & ((self: high_precision_time, presets:'%Y'|'%C'|'%y'|'%m'|
            '%B'|'%b'|'%d'|'%e'|'%H'|'%I'|'%M'|'%S'|'%p'|
            '%z'|'%Z'|'%j'|'%U'|'%W'|'%c'|'%x'|'%X'|'%%') -> string),
    type: (self: high_precision_time) -> "date_time.high_precision_time"
}
type __high_precision_time = {
    __sub: ((self: high_precision_time, other: high_precision_time) -> duration)
        & ((self: high_precision_time, other: duration) -> high_precision_time),
    __add: ((self: high_precision_time, other: high_precision_time) -> duration)
        & ((self: high_precision_time, other: duration) -> high_precision_time),
    __type: "date_time.high_precision_time"
}
--- Provides a standard set of utilities for working with date and time.
--- Internally implemented using `std::chrono` from C++, this module 
--- offers functionality to retrieve the current time, perform arithmetic with durations, and 
--- work with high-precision clocks. 
type date_time = {
    --- Retrieves the current system time as a `time_point`.
    --- This is typically equivalent to the current wall-clock time.
    now: () -> time,
    --- Constructs a `time_point` from the given date and time.
    --- @param year `number`: The year (e.g., 2025).
    --- @param month `number`: The month (1-12).
    --- @param day `number`: The day (1-31).
    --- @param hour `number`: The hour (0-23).
    --- @param minute `number`: The minute (0-59).
    --- @param second `number`: The second (0-59).
    --- @return `time_point`: The constructed time point.
    date_time: ((year: number, month: number, day: number,
        hour: number, minute: number, second: number) -> time),
    --- Constructs a `time_point` from the given date.
    --- @param year `number`: The year (e.g., 2025).
    --- @param month `number`: The month (1-12).
    --- @param day `number`: The day (1-31).
    --- @return `time_point`: The constructed time point.
    date: (year: number, month: number, day: number) -> time,
    --- Constructs a `duration` from the given time.
    --- @param hour `number`: The hour (0-23).
    --- @param minute `number`: The minute (0-59).
    --- @param second `number`: The second (0-59).
    --- @param millisecond `number?`: The millisecond.
    --- @param microsecond `number?`: The microsecond.
    --- @param nanosecond `number?`: The nanosecond.
    --- @return `duration`: The constructed duration.
    duration: ((...string) ->duration)
        & ((...chrono_literals) -> duration),
    --- Creates a `duration` representing the specified number of seconds.
    --- @param s number The number of seconds to represent.
    --- @return duration A duration object representing the given seconds.
    seconds: (s: number) -> duration,
    --- Creates a `duration` representing the specified number of nanoseconds.
    --- @param ns number The number of nanoseconds to represent.
    --- @return duration A duration object representing the given nanoseconds.
    nanoseconds: (ns: number) -> duration,
    --- Creates a `duration` representing the specified number of milliseconds.
    --- @param ms number The number of milliseconds to represent.
    --- @return duration A duration object representing the given milliseconds.
    milliseconds: (ms: number) -> duration,
    --- Creates a `duration` representing the specified number of microseconds.
    --- @param us number The number of microseconds to represent.
    --- @return duration A duration object representing the given microseconds.
    microseconds: (us: number) -> duration,
    --- Creates a `duration` representing the specified number of minutes.
    --- @param min number The number of minutes to represent.
    --- @return duration A duration object representing the given minutes.
    minutes: (min: number) -> duration,
    --- Creates a `duration` representing the specified number of days.
    --- @param d number The number of days to represent.
    --- @return duration A duration object representing the given days.
    days: (d: number) -> duration,
    --- Creates a `duration` representing the specified number of months.
    --- Note: The exact interpretation of a "month" may depend on your implementation.
    --- @param d number The number of months to represent.
    --- @return duration A duration object representing the given months.
    months: (d: number) -> duration,
    --- Creates a `duration` representing the specified number of hours.
    --- @param h number The number of hours to represent.
    --- @return duration A duration object representing the given hours.
    hours: (h: number) -> duration,
    --- Represents the system's wall-clock time (typically tied to the local timezone).
    --- This clock reflects real-world time and may be adjusted by the system.
    system_clock: {now: () -> time},
    --- A monotonic clock that cannot be adjusted and guarantees steady progression.
    --- Useful for measuring elapsed time.
    high_precision_clock: {now: () -> high_precision_time},
}
return dll.loadmodule("lumin-extras-date_time.dll") :: date_time
