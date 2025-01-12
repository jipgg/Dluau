declare dllimport: <A...,R...>(dll: string, symbol: string?) -> ((A...)->(R...))
declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare cospawn: <A..., R...>((A...)->(R...), R...) -> ()
declare cowait: (seconds: number) -> ()

declare dll: {
    loadmodule: <dllmodule>(dllname: string) -> dllmodule,
    loadfunction: <arg..., ret...>(dllname: string, funcname: string) -> ((arg...) -> (ret...)),
    findpath: (dllname: string) -> string?,
    unload: (dllname: string) -> (),
    loaded: () -> {string},
}
