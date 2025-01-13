local time = require("@extras/time")

local v = time.now()
print(`time is {v:format'%X'} {v} {typeof(v)}`)
local sb = time.high_precision_clock.now()
print(`precise_time is {sb} {typeof(sb)}`)
local date = time.date(8000, 10, 10)
print(`date: {date}`)

local e = time.seconds(10)
print(e)
print(time.now())
print(date:format('%D'))
print(e:format("%T"))
