local cc = require('console_codes')
print(`module is {cc}`)
for i, v in cc :: any do
    print(`   {i} = {v}`)
end
print({cc.blue})
print(dlimport.searchpath("dluaulib"))
local guitest = require('@guitest')
print(guitest)
