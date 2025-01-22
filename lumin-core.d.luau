declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare function scan(): string
export type parameter_type = 'int' | 'string' | 'double' | 'float' | 'void'

declare class dllmodule
    function dyncall_void(self, proc: string)
    dyncall_int: <Args...>(self: dllmodule, proc: string, Args...) -> number
    create_binding: <Return, Args...>(
        self: dllmodule,
        returns: parameter_type,
        func_name: string,
        ...parameter_type
    ) -> ((Args...)->Return)
end

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
