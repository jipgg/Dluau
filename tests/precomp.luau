local time = require '@time'
print(time.now())
assert(typeof(script.directory) == 'string')
assert(typeof(script.name) == 'string')
assert(typeof(script.path) == 'string')
local _ = script.directory:len()
print(script.directory, script.name)
local cc = require('console_codes')
print(`{cc.brightgreen}{script.name} tests successful{cc.reset}`)
