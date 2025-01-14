declare collectgarbage: (("collect") -> ()) & (("count") -> number)

declare class module
    name: string
    path: string
    getfunc: <As..., Rs...>(self: module, symbol: string) -> ((As...)->(Rs...))
end

declare dll: {
    getmodule: (name: string) -> module,
    findmodule: (name: string) -> module?,
    getfunc: <As..., Rs...>(module: module, symbol: string) -> ((As...)->(Rs...)),
}
