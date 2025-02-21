declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare nameof: <T>(variable: T) -> string
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

declare class dlmodule
    path: string
    name: string
    import_fn: <As..., Rs...>(self: dlmodule, symbol: string) -> ((As...)->(Rs...))
end

declare function dlload(target: string): dlmodule
declare function dlrequire(target: string): unknown
