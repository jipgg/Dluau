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
type cinterop_struct_field_description = {
    name: string,
    type: c_type,
    offset: number,
}
declare class c_aggregate
end
type cinterop_aggregate_field = {
    name: string,
    type: c_type,
    memory_offset: number,
    array_size: number,
}
type c_field_info = {
    type: c_type,
    memory_offset: number,
    array_size: number?,
}
declare class c_struct_info end
type cinterop = {
    native_type: {
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
    new_function_binding: (<Return, Args...>(module: dlmodule, returns: c_type | 'void' | c_aggregate, func_name: string, ...c_type|c_aggregate) -> ((Args...)->Return)),
    new_aggregate: (size: number, fields: {cinterop_aggregate_field}) -> c_aggregate, 
    create_struct_info: (memory_size: number, fields_info: {[string]: c_field_info}) -> c_struct_info,
    create_object: <T>(si: c_struct_info) -> T,
    get_struct_field: <T>(si: c_struct_info, ud: buffer, key: string) -> T,
    set_struct_field: <T>(si: c_struct_info, ud: buffer, key: string, value: T) -> T,
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

