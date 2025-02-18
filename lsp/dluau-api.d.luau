declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare nameof: <T>(v: T) -> string
declare lazyrequire: <T>(modulename: string) -> T
declare class userdata end
declare class opaque end
export type opaque_tagged<Tag> = {["@Tag"]: Tag} & opaque

export type os_date = {day: number, month: number, year: number, min: number, sec: number, hour: number, wday: number, yday: number, isdst: boolean}
export type os_date_opt = {day: number?, month: number?, year: number?, min: number?, sec: number?, hour: number?, wday: number?, yday: number?, isdst: boolean?}
type os_date_format_args = '%a'|'%A'|'%b'|'%B'|'%c'|'%d'|'%H'|'%I'|'%M'|'%m'|'%p'|'%S'|'%w'|'%x'|'%X'|'%Y'|'%y'|'%%'
type HINSTANCE = opaque_tagged<'HINSTANCE'>
type os_windows = {
    message_box: (message: string, title: string?, utype: number?) -> number,
    get_last_error: () -> number,
    format_message: (error_code: number) -> string,
    get_module_handle: (name: string?) -> HINSTANCE?,
    load_library: (lib_name: string) -> HINSTANCE?,
    free_library: (a: HINSTANCE) -> boolean,
}
type os = {
    execute: (command: string) -> number,
    exit: (code: number) -> never,
    getenv: (varname: string) -> string?,
    remove: (filename: string) -> (boolean | nil, string),
    rename: (oldfilename: string, newfilename: string) -> (boolean | nil, string),
    date: (('*t'|'!*t', time: number?) -> os_date)
        & ((format: string?, time: number?) -> string)
        & ((formatargs: os_date_format_args|string?, time: number?) -> string),
    time: (date: os_date_opt?) -> number,
    difftime: (t2: os_date | number, t1: os_date | number)->number,
    clock: ()->number,
    platform: 'windows'|'linux'|'macOS'|'freeBSD',
    windows: os_windows,
}
declare os: os

type task = {
    wait: (seconds: number?) -> (),
    spawn: <Ts...>(fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    defer: <Ts...>(fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    delay: <Ts...>(seconds: number, fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    cancel: (task: thread) -> (),
    waituntil: (task_to_finish: thread) -> thread,
    delayuntil: <Ts...>(task_to_finish: thread, fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    this: thread,
}
declare task: task

type script = {
    directory: string,
    path: string,
    name: string,
}
declare script: script

type callable = {__call: (any) -> never}

type print = {
    separator: string,
    noline: <Ts...>(Ts...)->(),
}
declare print: typeof(setmetatable({} :: print, {} :: callable)) & (<Ts...>(Ts...)->())
type scan = {
    buffer: ((buffersize: number) -> buffer) & ((b: buffer, len: number?)->buffer),
    number: () -> number?,
}
declare scan: typeof(setmetatable({} :: scan, {} :: callable)) & (()->string)

declare class dlmodule
    path: string
    name: string
    import: <As..., Rs...>(self: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
type dlimport = {
    require: (target: string) -> unknown,
    load: (target: string) -> dlmodule,
    pload: (target: string) -> (dlmodule?, string),
    searchpath: (dlname: string) -> string?,
    getmodules: () -> {dlmodule},
}
declare dlimport: dlimport 

