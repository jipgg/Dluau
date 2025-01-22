
export type fs = {
    current_directory: () -> fs_Dir,  
    temp_directory: () -> fs_Dir,  
    find_environment_variable: () -> string?,  
    path_exists: (path: string) -> boolean,
    Dir: {
        open: (path: string) -> fs_Dir,
        create: ((path: string) -> fs_Dir),
        remove: (path: string, recursive: boolean?) -> (),
    },
    File: {
        open: (path: string) -> fs_File,
        create: ((path: string) -> fs_File),
        remove: (path: string) -> (),
    },
}
return dll.cfunction(dll.load("xtm_filesystem"), "luminload_fs") () :: fs
