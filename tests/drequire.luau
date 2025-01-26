local e = drequire('module')
local f = drequire('module')
local yo = drequire('module/a')
print(yo)
print(`e: {e}, {e}`)
for i, v in e do
    print(`{i} = {v}`)
end
print(`f: {f}`)
for i, v in f do
    print(`{i} = {v}`)
end
print(`{f == e}`)
