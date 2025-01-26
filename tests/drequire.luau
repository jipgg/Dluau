local e = require('module')
local f = require('module')
local yo = require('module/a')
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
