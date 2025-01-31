local time = require '@time'

print(time.now())
assert(typeof(script.directory) == 'string')
assert(typeof(script.name) == 'string')
assert(typeof(script.path) == 'string')
local _ = script.directory:len()
print(script.directory, script.name)
print.text_color = 'green'
print(`{script.name} tests success`)
print.reset_colors()
