local e = require('module')
local time = require('@require/qol2/time')
print(time, time.now())

print.text_color = "cyan"
local now = time.now()
print(now:format('%X'))
print.reset_colors()
task.wait(1)
task.wait(2)
task.wait(3)
task.wait(1)
print.reset_colors()
print("YOOOOOOOOOOOOOOOO")
print.text_color = "cyan"
print(time.now():format('%X'))
print(time.now() - now);
print.reset_colors()
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
