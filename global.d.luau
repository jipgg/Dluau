declare dllimport: <A...,R...>(dll: string, symbol: string?) -> ((A...)->(R...))
declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare cospawn: <A..., R...>((A...)->(R...), R...) -> ()
declare cowait: (seconds: number) -> ()
