declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare nameof: <T>(v: T) -> string
export type param = 'int' | 'string' | 'double' | 'float'| 'short' |'long' | 'long long'| 'pointer'
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

declare class dlmodule
    path: string
    name: string
    rawcbinding: (<Return, Args...>(self: dlmodule, returns: param | 'void', func_name: string, ...param) -> ((Args...)->Return))
    importfunction: <As..., Rs...>(module: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
type dlimport = {
    require: (target: string) -> unknown,
    load: (target: string) -> dlmodule,
    pload: (target: string) -> (dlmodule?, string),
    searchpath: (dlname: string) -> string?,
    getmodules: () -> {dlmodule},
}
declare dlimport: dlimport 

