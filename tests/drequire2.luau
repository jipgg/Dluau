local e = require('module')
local f = require('module')
print(`e2: {e}, {e}`)
for i, v in e do
    print(`{i} = {v}`)
end
print(`f2: {f}`)
for i, v in f do
    print(`{i} = {v}`)
end
print(`{f == e}`)
