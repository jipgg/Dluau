declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare nameof: <T>(variable: T) -> string
declare lazyrequire: <T>(module_name: string) -> T
declare class userdata end
declare class opaque end

type task = {
    wait: (seconds: number?) -> (),
    spawn: <Ts...>(fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    defer: <Ts...>(fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    delay: <Ts...>(seconds: number, fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    cancel: (task: thread) -> (),
    wait_for: (for_this: thread) -> thread,
    do_after: <Ts...>(after_this: thread, fn_or_thread: ((Ts...)->()) | thread, Ts...) -> thread,
    this: thread,
}
declare task: task

type script = {
    directory: string,
    path: string,
    name: string,
}

declare class dlmodule
    path: string
    name: string
    import_fn: <As..., Rs...>(self: dlmodule, symbol: string) -> ((As...)->(Rs...))
end
type dlimport = {
    dlrequire: (target_dl: string) -> unknown,
    load: (target_dl: string) -> dlmodule,
    try_load: (target_dl: string) -> (dlmodule?, string),
    search_path: (dl_name: string) -> string?,
    loaded_modules: () -> {dlmodule},
}
declare dlimport: dlimport 

