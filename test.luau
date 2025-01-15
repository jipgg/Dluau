local time = require("@extras/time")
print(`module is {dll.getmodule("time.lumin.dll")}`)
print(`path is {dll.getmodule("time.lumin.dll").path}`)
print(`name is {dll.getmodule("time.lumin.dll").name}`)

local cwd = fs.getcwd()
local temp = fs.makedir("temp")
local temp2 = fs.makedir("temp2")
local file = fs.file('./test.luau')

for ln in file:eachline() do
    print(`line: {ln}`)
end

temp2.parent = temp
print(`parent: {temp2.parent}`)
print(temp)
print(cwd)

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
