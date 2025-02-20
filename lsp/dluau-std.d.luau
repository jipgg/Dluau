-- interfaces
declare class reader
    function get_line(self, delimiter_char: string?): string?
    function each_line(self, delimiter_char: string?): (() -> string?)
    function read(self, buf: buffer): (number, buffer)
    function read_all(self): buffer
    function get(self): number
    function peek(self): number
    function ty_offset(self, new_pos: number): boolean
    function ignore(self, count: number?)
    function free(self): ()
    eof: boolean
    offset: number
    last_bytes_count: number
end
declare class writer
    function write(self, buf: buffer): ()
    --print: <TyArgs...>(self: writer, TyArgs...) -> (),
    function put(self, char: number)
    function reset(self): ()
    function try_offset(self, new_pos: number): boolean
    function free(self): ()
    eof: boolean
    offset: number
end
-- std.fs
export type copy_options = 'recursive' |'none' |'update existing' |'skip existing' |'overwrite existing' |'create symlinks' |'copy symlinks' |'skip symlinks' |'directories only' |'create hard links'
    
declare class symlink
    link: string
    path: string
    parent: directory
    name: string
end
declare class file
    function writer(self): writer
    function append_writer(self): writer
    function reader(self): reader
    path: string
    name: string
    extension: string
    stem: string
    parent: directory
end
declare class directory
    function empty(self): boolean
    function contains(self, child_path: string): boolean
    function each_child(self): (() -> pathlike?)
    function each_descendant(self): (() -> pathlike?)
    function open_file(self, subpath: string): file
    function make_file(self, subpath: string): file
    function open_directory(self, subpath: string): directory
    function make_directory(self, subpath: string): directory
    path: string
    name: string
    parent: directory
end
export type pathlike = directory | symlink | file

type std_fs = {
    canonical: (path: string) -> string,
    weakly_canonical: (path: string) -> string,
    proximate: (path: string, base: string?) -> string,
    relative: (path: string, base: string?) -> string,
    absolute: (path: string) -> string,
    each_child: (path: string) -> (()-> pathlike?),
    remove: (path: pathlike | string) -> (), 
    remove_all: (path: pathlike | string) -> (), 
    open_directory: (path: string) -> directory,
    open_file: (path: string) -> file,
    make_directory: (path: string) -> directory,
    make_file: (path: string) -> file,
    make_symlink: (to: pathlike | string, path: string) -> symlink,
    each_descendant: (path: string) -> (() -> pathlike?),
    current_directory: () -> directory,  
    temp_directory: () -> directory,  
    find_environment_variable: (varname: string) -> string?,  
    exists: (path: string) -> boolean,
    copy: ((from: file, to: string) -> file)
        & ((from: directory, to: string) -> directory)
        & ((from: symlink, to: string) -> symlink)
        & ((from: string, to: string) -> ())
        & ((from: file, to: string, ...copy_options) -> file)
        & ((from: directory, to: string, ...copy_options) -> directory)
        & ((from: symlink, to: string, ...copy_options) -> symlink)
        & ((from: string, to: string, ...copy_options) -> ()),
}
-- std.time
export type time_span_format_args = '%H'|'%M'|'%S'|'%T'|'%R'|'%p'
declare class time_span
    format: ((self: time_span, fmt: string) -> string)
        & ((self: time_span, time_span_format_args) -> string)
    total_nanoseconds: number
    total_microseconds: number
    total_milliseconds: number
    total_minutes: number
    total_hours: number
    function __sub(self, other: time_span): time_span
    function __add(self, other: time_span): time_span
end
export type time_point_format_args = '%Y'|'%C'|'%y'|'%m'|
            '%B'|'%b'|'%d'|'%e'|'%H'|'%I'|'%M'|'%S'|'%p'|
            '%z'|'%Z'|'%j'|'%U'|'%W'|'%c'|'%x'|'%X'|'%%'

declare class time_point
    function change_zone(self, time_zone: string): time_point
    format: ((self: time_point, fmt: string) -> string)
        & ((self: time_point, time_point_format_args) -> string)
    time_zone: string
    zone_abbreviation: string
    year: number
    month: number
    day: number
    hour: number
    minute: number
    second: number
    millisecond: number
    __sub: ((self: time_point, other: time_point) -> time_span)
        & ((self: time_point, other: time_span) -> time_point)
    function __add(self, other: time_span): time_point
end
declare class nano_time_point
    time_zone: string
    zone_abbreviation: string
    hour: number
    minute: number
    second: number
    millisecond: number
    microsecond: number
    nanosecond: number
    __sub: ((self: nano_time_point, other: nano_time_point) -> time_span)
        & ((self: nano_time_point, other: time_span) -> nano_time_point)
    function __add(self, other: time_span): nano_time_point
end
export type std_time = {
    current_zone: string,
    now: (timezone: string?) -> time_point,
    nano_now: () -> nano_time_point,
    utc_now: () -> time_point,
    from_datetime: ((year: number, month: number, day: number,
        hour: number, minute: number, second: number) -> time_point),
    from_date: (year: number, month: number, day: number) -> time_point,
    seconds: (s: number) -> time_span,
    nanoseconds: (ns: number) -> time_span,
    milliseconds: (ms: number) -> time_span,
    microseconds: (us: number) -> time_span,
    minutes: (min: number) -> time_span,
    days: (d: number) -> time_span,
    months: (d: number) -> time_span,
    hours: (h: number) -> time_span,
}
-- process
declare class pid_t
    function __tostring(self): string
    function __eq(self, other: pid_t): boolean
end
declare class child
    function join(self): ()
    function terminate(self): ()
    function detach(self): ()
    function wait(self): ()
    id: pid_t
    joinable: boolean
    exit_code: number
    native_exit_code: number
    running: boolean
end
type std_process = {
    search_path: (exename: string) -> string?,
    system: ((command: string) -> number) & ((exe: string, ...string) -> number),
    spawn: ((command: string) -> ()) & ((exe: string, ...string) -> ()),
    child: ((command: string) -> child)
        & ((exe: string, ...string) -> child)
        & ((id: pid_t) -> child),
}
-- cinterop
declare class struct_info
    function to_pointer(self, obj: unknown): opaque
    get_field: <Ty>(
        self: struct_info,
        data: opaque,
        key: string,
        arr_index: number?) -> Ty
    set_field: <Ty>(
        self: struct_info,
        data: opaque,
        key: string,
        val: Ty,
        arr_index: number?
    ) -> ()
    new_instance: <Ty>(self: struct_info) -> Ty
end

export type field_info_type = 'bool'|'double'|'char*'|'void*'|'uint'|'ulong'|'ushort'|'char'|'int'|'float'|'short'|'long'|'uchar'
export type field_info = {
    type: field_info_type,
    memory_offset: number,
    array_size: number?,
}
type field_info_map = {[string]: field_info}
type std_cinterop = {
    struct_info: (
        memory_size: number,
        fields: field_info_map,
        metatable: any?
    ) -> struct_info,
    bind_fn: (<Return, Args...>(
        module: dlmodule,
        returns: field_info_type | 'void' | struct_info,
        func_name: string,
        ...field_info_type | struct_info
    ) -> ((Args...)->Return)),
}
type std_os_windows = {
    message_box: (message: string, title: string?, utype: number?) -> number,
    get_last_error: () -> number,
    format_message: (error_code: number) -> string,
    get_module_handle: (name: string?) -> opaque?,
    load_library: (lib_name: string) -> opaque?,
    free_library: (a: opaque) -> boolean,
    MB_ABORTRETRYIGNORE: number,
    MB_CANCELTRYCONTINUE: number,
    MB_HELP: number,
    MB_OK: number,
    MB_OKCANCEL: number,
    MB_RETRYCANCEL: number,
    MB_YESNO: number,
    MB_YESNOCANCEL: number,
    MB_ICONEXCLAMATION: number,
    MB_ICONWARNING: number,
    MB_ICONINFORMATION: number,
    MB_ICONASTERISK: number,
    MB_ICONQUESTION: number,
    MB_ICONSTOP: number,
    MB_ICONERROR: number,
    MB_ICONHAND: number,
    MB_DEFBUTTON1: number,
    MB_DEFBUTTON2: number,
    MB_DEFBUTTON3: number,
    MB_DEFBUTTON4: number,
    MB_APPLMODAL: number,
    MB_SYSTEMMODAL: number,
    MB_TASKMODAL: number,
    MB_DEFAULT_DESKTOP_ONLY: number,
    MB_RIGHT: number,
    MB_RTLREADING: number,
    MB_SETFOREGROUND: number,
    MB_TOPMOST: number,
    MB_SERVICE_NOTIFICATION: number,
    IDABORT: number,
    IDCANCEL: number,
    IDCONTINUE: number,
    IDIGNORE: number,
    IDNO: number,
    IDOK: number,
    IDRETRY: number,
    IDTRYAGAIN: number,
    IDYES: number,
}
type std_os = {
    execute: (command: string) -> number,
    exit: (code: number) -> never,
    platform: 'windows'|'linux'|'macOS'|'freeBSD',
    windows: std_os_windows,
}
type std_io = {
    escape_code: (escape_sequence: string) -> string, 
    scan: () -> string,
    read: (to_buffer: buffer) -> (),
    write: ((from_buffer: buffer) -> ()),
    print: <T>(output: T) -> (),
    println: <T>(output: T) -> (),
}
type std = {
    io: std_io,
    os: std_os,
    fs: std_fs,
    time: std_time,
    process: std_process,
    cinterop: std_cinterop
}

declare std: std
