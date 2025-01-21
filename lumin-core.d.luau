declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare class dllmodule end
declare function scan(): string

declare meta: {
    script_directory: () -> string,
    script_name: () -> string,
    command_line_arguments: () -> {string}?,
    dll: {
        load: (dllname: string) -> dllmodule,
        find: (dllname: string) -> dllmodule?,
        cfunction: <As..., Rs...>(module: dllmodule, symbol: string) -> ((As...)->(Rs...)),
    }
}
declare dll: {
    load: (dllname: string) -> dllmodule,
    find: (dllname: string) -> dllmodule?,
    cfunction: <As..., Rs...>(module: dllmodule, symbol: string) -> ((As...)->(Rs...)),
}

export type Project_configuration = {
    sources: {string} | string,
    optimization_level: number?,
    debug_level: number?,
    host: ('console' | 'general')?,
}
