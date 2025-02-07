declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare nameof: <T>(v: T) -> string
declare class userdata end

export type osdate = {day: number, month: number, year: number, min: number, sec: number, hour: number, wday: number, yday: number, isdst: boolean}
export type osdateopt = {day: number?, month: number?, year: number?, min: number?, sec: number?, hour: number?, wday: number?, yday: number?, isdst: boolean?}
type osdateformatargs = '%a'|'%A'|'%b'|'%B'|'%c'|'%d'|'%H'|'%I'|'%M'|'%m'|'%p'|'%S'|'%w'|'%x'|'%X'|'%Y'|'%y'|'%%'
type os = {
    execute: (command: string) -> number,
    exit: (code: number) -> never,
    getenv: (varname: string) -> string?,
    remove: (filename: string) -> (boolean | nil, string),
    rename: (oldfilename: string, newfilename: string) -> (boolean | nil, string),
    date: (('*t'|'!*t', time: number?) -> osdate)
        & ((format: string?, time: number?) -> string)
        & ((formatargs: osdateformatargs|string?, time: number?) -> string),
    time: (date: osdateopt?) -> number,
    difftime: (t2: osdate | number, t1: osdate | number)->number,
    clock: ()->number,
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

export type c_type =
    'boolean'|
    'number'|
    'string'|
    'userdata'|
    'c_uint'|
    'c_ulong'|
    'c_ushort'|
    'c_char'|
    'c_int'|
    'c_float'|
    'c_short'|
    'c_long'|
    'c_uchar'
declare class c_int end
declare class c_uint end
declare class c_char end
declare class c_uchar end
declare class c_short end
declare class c_ushort end
declare class c_long end
declare class c_ulong end
declare class c_float end

declare class dlmodule
    path: string
    name: string
    import: <As..., Rs...>(self: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
export type cinterop_fieldinfo = {
    type: c_type,
    memoffset: number,
    arrsize: number?
}
declare class structinfo end
type cinterop = {
    native: {
        int: (number | string) -> c_int,
        uint: (number | string) -> c_uint,
        char: (number | string) -> c_char,
        uchar: (number | string) -> c_uchar,
        short: (number | string) -> c_short,
        ushort: (number | string) -> c_ushort,
        long: (number | string) -> c_long,
        ulong: (number | string) -> c_ulong,
        float: (number) -> c_float,
    },
    bindfunction: (<Return, Args...>(module: dlmodule, returns: c_type | 'void' | structinfo, func_name: string, ...c_type|structinfo) -> ((Args...)->Return)),
    struct: {
        newinfo: (memsize: number, fieldsinfo: {[string]: cinterop_fieldinfo}, metatable: any?) -> structinfo,
        getfield: <T>(s: structinfo, data: buffer | userdata, key: string) -> T,
        setfield: <T>(s: structinfo, data: buffer | userdata, key: string, newval: T) -> T,
    },
}
type dlimport = {
    require: (target: string) -> unknown,
    load: (target: string) -> dlmodule,
    pload: (target: string) -> (dlmodule?, string),
    searchpath: (dlname: string) -> string?,
    getmodules: () -> {dlmodule},
    cinterop: cinterop,
}
declare dlimport: dlimport 

