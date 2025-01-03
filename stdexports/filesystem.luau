export type Path = typeof(setmetatable({} :: Path_raw, {} :: Path_meta))
type Path_meta = {
    __div: (self: Path, self: Path | string) -> Path,
    __tostring: (self: Path) -> string,
}
type Path_raw = {
    parent_path: (self: Path) -> Path,
    absolute: (self: Path) -> Path,
    extension: (self: Path) -> string,
    filename: (self: Path) -> string,
    has_extension: (self: Path) -> boolean,
    has_filename: (self: Path) -> boolean,
    is_directory: (self: Path) -> boolean,
    stem: (self: Path) -> string,
    has_stem: (self: Path) -> boolean,
    replace_extension: (self: Path, replacement: string) -> Path,
    replace_filename: (self: Path, replacement: string) -> Path,
    exists: (self: Path) -> boolean,
}
export type Module = {
    -- absolute path
    absolute: (path: string) -> string,
    -- current working directory path
    current_path: () -> string,
    -- temp directory
    temp_directory_path: () -> string,
    relative: (path: string, from: string?) -> string,
    children: (path: string) -> {string},
    descendants: (path: string) -> {string},
    -- Path class constructor
    Path: (path: string) -> Path,
}
return cimport("halua-std.dll", "stdfilesystem")() :: Module
