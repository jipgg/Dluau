declare collectgarbage: (("collect") -> ()) & (("count") -> number)

declare dll: {
    loadmodule: <dllmodule>(dllname: string) -> dllmodule,
    loadfunction: <arg..., ret...>(dllname: string, funcname: string) -> ((arg...) -> (ret...)),
    findpath: (dllname: string) -> string?,
    unload: (dllname: string) -> (),
    loaded: () -> {string},
}
