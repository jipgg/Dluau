declare collectgarbage: (("collect") -> ()) & (("count") -> number)
declare test: (subject: string, block: ()->()) -> ()

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

declare class directory
    parent: directory
    exists: boolean
    name: string
    function iterator(self): (()->(directory | file)?)
    function recursive_iterator(self): (()->(directory | file)?)
end

declare class reader
    function getline(self): string?
    eachline: ((self: reader) -> (()->string?))
    function __concat(self, v: nil | number): (()->string?)
    eof: boolean
end

declare class file
    parent: directory
    stem: string
    extension: string
    filename: string
    function eachline(self): (()->string?)
    function reader(self): reader
end

declare fs: {
    getcwd: () -> directory,  
    file: (path: string) -> file,
    makedir: ((path: string) -> directory)
        & ((dir: directory) -> directory),
    directory: (path: string) -> directory,
}
