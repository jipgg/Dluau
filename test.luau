local fs = require("@std/fs")
type Path = {
    hello: (self: Path) -> (),
    parent_path: (self: Path) -> Path,
    absolute: (self: Path) -> Path,
}
local Path = cimport("halua-std.dll", "Path") :: (path: string) -> Path
local my_path = Path("std")
print(my_path, typeof(my_path))
my_path:hello()
local parent_path = my_path:absolute():parent_path();
print(`parent {parent_path}`)
for i, v in fs do
    print(`[fs]: {i} = {v}`)
end

