declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare function scan(): string
export type param = 'int' | 'string' | 'double' | 'float'


declare meta: {
    script_directory: () -> string,
    script_name: () -> string,
    command_line_arguments: () -> {string}?,
}

declare class dlmodule
    bind: (<Return, Args...>(self: dlmodule, returns: param | 'void', func_name: string, ...param) -> ((Args...)->Return))
    lua_cfunction: <As..., Rs...>(module: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
declare dlimport: {
    load: (dlname: string) -> dlmodule,
    try_load: (dlname: string) -> dlmodule | string,
    search_for: (dlname: string) -> string?,
}

export type Project_configuration = {
    sources: {string} | string,
    optimization_level: number?,
    debug_level: number?,
    host: ('console' | 'general')?,
}
