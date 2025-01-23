declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare function scan(): string
export type parameter_type = 'int' | 'string' | 'double' | 'float' | 'void'


declare meta: {
    script_directory: () -> string,
    script_name: () -> string,
    command_line_arguments: () -> {string}?,
}

declare class dlmodule
    create_binding: <Return, Args...>(
        self: dlmodule,
        returns: parameter_type,
        func_name: string,
        ...parameter_type
    ) -> ((Args...)->Return)
    lua_cfunction: <As..., Rs...>(module: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
declare dlimport: {
    load: (dlname: string) -> dlmodule,
    try_load: (dlname: string) -> dlmodule?,
    search_for: (dlname: string) -> string?,
}

export type Project_configuration = {
    sources: {string} | string,
    optimization_level: number?,
    debug_level: number?,
    host: ('console' | 'general')?,
}
