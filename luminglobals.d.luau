declare dllimport: <Arg..., Ret...>(dll: string, symbol: string) -> ((Arg...) -> (Ret...))
declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare script: {
    absolute_path: string,
}
declare class opaque end
