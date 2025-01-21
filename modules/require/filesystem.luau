export type path = typeof(setmetatable({} :: _path, {} :: __path))
type __path = {
    __div: (self: path, self: path | string) -> path,
    __tostring: (self: path) -> string,
}
type _path = {
    --- parent directory
    parent_path: (self: path) -> path,
    --- returns the absolute path
    absolute: (self: path) -> path,
    --- returns the extension if it exists
    --- example: ".luau"
    extension: (self: path) -> string,
    --- returns the file name if it exists
    --- example: "myfile.luau"
    filename: (self: path) -> string,
    has_extension: (self: path) -> boolean,
    has_filename: (self: path) -> boolean,
    --- returns true if the path is a directory/folder
    is_directory: (self: path) -> boolean,
    --- returns the stem of a file
    --- example: "myfile.luau" -> "myfile"
    stem: (self: path) -> string,
    has_stem: (self: path) -> boolean,
    replace_extension: (self: path, replacement: string) -> path,
    replace_filename: (self: path, replacement: string) -> path,
    --- returns true if the path actually exists
    exists: (self: path) -> boolean,
}
export type filesystem = {
    --- absolute path
    absolute: (path: string) -> string,
    -- current working directory path
    current_path: () -> string,
    -- temp directory
    temp_directory_path: () -> string,
    relative: (path: string, from: string?) -> string,
    children: (path: string) -> {string},
    descendants: (path: string) -> {string},
    -- Path class constructor
    path: (path: string) -> path,
}
return dll.loadmodule('lumin-extras-filesystem.dll') :: filesystem
