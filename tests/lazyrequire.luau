type module = typeof(require('@gpm/time'))
local lazy = lazyrequire("@gpm/time") :: module

print(lazy.current_zone)
print(lazy)
local now = lazy.now()
local time = require('@gpm/time')
assert(rawequal(time.now, lazy.now)) -- same index instance
local e = lazy.days(1)
print(e)
print(now)
