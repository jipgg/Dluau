declare collectgarbage: (("collect") -> ()) & (("count") -> number)
-- declare wait: (length: number) -> ()
declare function scan(): string
declare script: {
    file: fsFile,
    path: string,
}
declare meta: {
    script: string,
    argv: {string},
    dll: {
        Module: {
            load: (dllname: string) -> dllModule,
            find: (dllname: string) -> dllModule?,
        },
        get_module: (name: string) -> dllModule,
        find_module: (name: string) -> dllModule?,
        cfunction: <As..., Rs...>(module: dllModule, symbol: string) -> ((As...)->(Rs...)),
    },
}
declare class dllModule
    name: string
    path: string
    cfunction: <As..., Rs...>(self: dllModule, symbol: string) -> ((As...)->(Rs...))
end

declare dll: {
    Module: {
        load: (dllname: string) -> dllModule,
        find: (dllname: string) -> dllModule?,
    },
    get_module: (name: string) -> dllModule,
    find_module: (name: string) -> dllModule?,
    cfunction: <As..., Rs...>(module: dllModule, symbol: string) -> ((As...)->(Rs...)),
}

declare class ioReader
    get_line: ((self: ioReader) -> string?)
        & ((self: ioReader, delimiter_char: string) -> string?)
    line_iterator: ((self: ioReader) -> (()->string?))
        & ((self: ioReader, delimiter_char: string) -> (()->string?))
    read: (self: ioReader, buf: buffer) -> (number, buffer)
    read_all: (self: ioReader) -> buffer
    get: (self: ioReader) -> number
    peek: (self: ioReader) -> number
    try_offset: (self: ioReader, new_pos: number) -> boolean
    ignore: ((self: ioReader) -> ())
        & ((self: ioReader, count: number) -> ())
    eof: boolean
    offset: number
    last_bytes_count: number
end
declare class ioWriter
    write: (self: ioWriter, buf: buffer) -> ()
    print: <Ts...>(self: ioWriter, Ts...) -> ()
    put: (self: ioWriter, char: number) -> ()
    reset: (self: ioWriter) -> ()
    try_offset: (self: ioWriter, new_pos: number) -> boolean
    eof: boolean
    offset: number
end
declare class fsSymlink end

declare class fsFile
    parent_directory: fsDir
    name: string
    extension: string
    stem: string
    function writer(self, append_mode: boolean?): ioWriter
    function reader(self): ioReader
end

declare class fsDir
    parent_directory: fsDir
    name: string
    function is_empty(self): boolean
    function contains(self, child_name: string): boolean
    function iterator(self): (()->(fsDir | fsFile)?)
    function recursive_iterator(self): (()->(fsDir | fsFile)?)
    function __div(self, other: fsFile | fsDir | string): string
end
declare fs: {
    current_directory: () -> fsDir,  
    temp_directory: () -> fsDir,  
    find_environment_variable: () -> string?,  
    path_exists: (path: string) -> boolean,
    Dir: {
        open: (path: string) -> fsDir,
        create: ((path: string) -> fsDir),
        remove: (path: string, recursive: boolean?) -> (),
    },
    File: {
        open: (path: string) -> fsFile,
        create: ((path: string) -> fsFile),
        remove: (path: string) -> (),
    },
}
type ioWriter_options = {
    append: boolean?
}
declare io: {
        Writer: ((open: fsFile, opts: ioWriter_options?) -> ioWriter)
            & ((open: buffer) -> ioWriter),
        Reader: ((open: fsFile) -> ioReader)
            & ((open: buffer) -> ioReader),
}
declare proc: {
    args: {string},
    system: (command: string) -> number,
}
export type Project_configuration = {
    sources: {string} | string,
    optimization_level: number?,
    debug_level: number?,
    console: ("popup"|"attach"|"none"|"popup-pause")?,
}
