local time = require 'time'

print(time.high_precision_clock.now())
print(time.now())
print(`{meta.os_name}`)
print(`{meta.debug_level}`)
print(`{meta.optimization_level}`)
print(script.directory, script.name)

