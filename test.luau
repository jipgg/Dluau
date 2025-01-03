local fs = require("@std/filesystem")
local console = require("@std/console")
local Color = console.Color
console.set_foreground_color(Color.Red)
for i, v in Color do
    print(`{i} = {v}`)
end
console.ansi("[0m")
local p: fs.Path = fs.Path("std")
local paren = p:absolute():parent_path();
print(paren)
print(p:exists())
