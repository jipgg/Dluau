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
    function flush(self): ()
    print: <TyArgs...>(self: writer, TyArgs...) -> ()
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
    function read_contents(self): string
    function append(self, ln: string): ()
    function overwrite(self, contents: string): ()
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

export type std_fs = {
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
export type std_process = {
    search_path: (exename: string) -> string?,
    system: ((command: string) -> number) & ((exe: string, ...string) -> number),
    spawn: ((command: string) -> ()) & ((exe: string, ...string) -> ()),
    child: ((command: string) -> child)
        & ((exe: string, ...string) -> child)
        & ((id: pid_t) -> child),
}
-- cinterop
declare class struct_info
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
export type field_info_map = {[string]: field_info}
export type std_cinterop = {
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
export type std_os_api_windows = {
    message_box: (message: string, title: string?, utype: number?) -> number,
    get_last_error: () -> number,
    format_message: (error_code: number) -> string,
    get_module_handle: (name: string?) -> opaque?,
    load_library: (lib_name: string) -> opaque?,
    free_library: (a: opaque) -> boolean,
    get_async_key_state: (vkey: number) -> number,
    get_key_state: (vkey: number) -> number,
}
export type virtual_key_args = 'mouse1'|'mouse2'|'mouse3'|'mouse4'|'mouse5'|'backspace'|'tab'|'clear'|'enter'|'shift'|'ctrl'|'alt'|'pause'|'caps lock'|'esc'|'space'|'page down'|'page up'|'end'|'home'|'left'|'right'|'up'|'down'|'select'|'print'|'execute'|'print screen'|'ins'|'del'|'help'|'0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'a'|'b'|'c'|'d'|'e'|'f'|'g'|'h'|'i'|'j'|'k'|'l'|'m'|'n'|'o'|'p'|'u'|'v'|'w'|'x'|'y'|'z'|'lsuper'|'rsuper'|'num0'|'num1'|'num2'|'num3'|'num4'|'num5'|'num6'|'num7'|'num8'|'num9'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|'lshift'|'rshift'|'lctrl'|'rctrl'|'lalt'|'ralt'
export type std_os = {
    execute: (command: string) -> number,
    exit: (code: number) -> never,
    platform: 'windows'|'linux'|'macOS'|'freeBSD',
    sleep: (milliseconds: number) -> (),
    keydown: ((key: string) -> boolean)
        & ((key: virtual_key_args) -> boolean),
    messagebox: ((message: string, title: string?) -> 'ok')
        & ((message: string, title: string?, 'ok cancel') -> 'ok' | 'cancel')
        & ((message: string, title: string?, 'yes no') -> 'yes' | 'no')
        & ((message: string, title: string?, 'abort retry ignore') -> 'abort' | 'retry' | 'ignore')
        & ((message: string, title: string?, 'retry cancel') -> 'retry'|'cancel')
        & ((message: string, title: string?, 'cancel try continue') -> 'cancel'|'try'|'continue')
        & ((message: string, title: string?, 'yes no cancel') -> 'yes'|'no'|'cancel'),
    api: {
        windows: std_os_api_windows,
    }
}
export type std_io = {
    escape_code: (escape_sequence: string) -> string, 
    scan: () -> string,
    read: (to_buffer: buffer) -> (),
    write: ((from_buffer: buffer) -> ()),
    print: <T>(output: T) -> (),
    println: <T>(output: T) -> (),
}
export type json_value = string | json_array | json_object | buffer | number | nil | boolean
export type json_array = {json_value}
export type json_object = {[string]: json_value}
export type std_json = {
    parse: (json_source: string) -> {json_value},
}
type std = {
    json: std_json,
    io: std_io,
    os: std_os,
    fs: std_fs,
    time: std_time,
    process: std_process,
    cinterop: std_cinterop
}

declare std: std
