declare collectgarbage: (("collect") -> ()) & (("count") -> number)

declare class module
    name: string
    path: string
end

declare dll: {
    getmodule: (name: string) -> module,
    findmodule: (name: string) -> module?,
    getfunction: <As..., Rs...>(module, symbol: string) -> ((As...)->(Rs...)),
}
