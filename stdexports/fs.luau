export type Module = {
    absolute: (path: string) -> string,
    current_path: () -> string,
    temp_directory_path: () -> string,
    relative: (path: string, from: string?) -> string,
    children: (path: string) -> {string},
    descendants: (path: string) -> {string},
}
return cimport("halua-std.dll", "filesystem")() :: Module
