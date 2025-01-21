declare class fs_Reader
    get_line: ((self: fs_Reader) -> string?)
        & ((self: fs_Reader, delimiter_char: string) -> string?)
    line_iterator: ((self: fs_Reader) -> (()->string?))
        & ((self: fs_Reader, delimiter_char: string) -> (()->string?))
    read: (self: fs_Reader, buf: buffer) -> (number, buffer)
    read_all: (self: fs_Reader) -> buffer
    get: (self: fs_Reader) -> number
    peek: (self: fs_Reader) -> number
    try_offset: (self: fs_Reader, new_pos: number) -> boolean
    ignore: ((self: fs_Reader) -> ())
        & ((self: fs_Reader, count: number) -> ())
    eof: boolean
    offset: number
    last_bytes_count: number
end
declare class fs_Writer
    write: (self: fs_Writer, buf: buffer) -> ()
    print: <Ts...>(self: fs_Writer, Ts...) -> ()
    put: (self: fs_Writer, char: number) -> ()
    reset: (self: fs_Writer) -> ()
    try_offset: (self: fs_Writer, new_pos: number) -> boolean
    eof: boolean
    offset: number
end

declare class fs_File
    parent_directory: fs_Dir
    name: string
    extension: string
    stem: string
    function writer(self, append_mode: boolean?): fs_Writer
    function reader(self): fs_Reader
end

declare class fs_Dir
    parent_directory: fs_Dir
    name: string
    function is_empty(self): boolean
    function contains(self, child_name: string): boolean
    function iterator(self): (()->(fs_Dir | fs_File)?)
    function recursive_iterator(self): (()->(fs_Dir | fs_File)?)
    function __div(self, other: fs_File | fs_Dir | string): string
end
