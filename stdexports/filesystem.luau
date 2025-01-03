local FILESYSTEM_DLL = "halua.filesystem.dll"
export type Path = typeof(setmetatable({} :: Path_raw, {} :: Path_meta))
type Path_meta = {
    __tostring: (self: Path) -> string,
    __div: (self: Path | string, other: Path | string) -> Path,
}
type Path_raw = {
}
return {
    absolute = cimport(FILESYSTEM_DLL, "absolute") :: (path: string) -> string,
    current_path = cimport(FILESYSTEM_DLL, "current_path") :: () -> string,
    temp_directory_path = cimport(FILESYSTEM_DLL, "temp_directory_path") :: () -> string,
    relative = cimport(FILESYSTEM_DLL, "relative") :: (path: string, from: string?) -> string,
    children = cimport(FILESYSTEM_DLL, "children") :: (path: string) -> {string},
    descendants = cimport(FILESYSTEM_DLL, "descendants") :: (path: string) -> {string},
    exists = cimport(FILESYSTEM_DLL, "exists") :: (path: string) -> boolean,
    is_directory = cimport(FILESYSTEM_DLL, "is_directory") :: (path: string) -> boolean,
    is_fifo = cimport(FILESYSTEM_DLL, "is_fifo") :: (path: string) -> boolean,
    is_regular_file = cimport(FILESYSTEM_DLL, "is_regular_file") :: (path: string) -> boolean,
    is_character_file = cimport(FILESYSTEM_DLL, "is_character_file") :: (path: string) -> boolean,
    is_block_file = cimport(FILESYSTEM_DLL, "is_block_file") :: (path: string) -> boolean,
    is_symlink = cimport(FILESYSTEM_DLL, "is_symlink") :: (path: string) -> boolean,
}
