type module = typeof(require('@gpm/time'))
local lazy = lazyrequire("@gpm/time") :: module
local dummy = require('dummy_module')
print(dummy.submodule.name)

print(lazy.current_zone)
print(lazy)
local now = lazy.now()
local time = require('@gpm/time')
assert(rawequal(time.now, lazy.now)) -- same index instance
local e = lazy.days(1)
print(e)
print(now)
