local file = fs.File.open('./test.luau')
local console = require('@extras/console')
local cc = console.codes

for i, v in proc.args do
    print(`{cc.fg.magenta}[{i} arg] = {v}{cc.reset}`)
end
print("naame? ")
--local v = scan()
--print(`NAME IS {v}`)

for line in file:reader():line_iterator() do
    print(line)
end
