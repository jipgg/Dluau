local console = require("@stdlib/console")
local desktop = require("@stdlib/desktop")
local dt = require("@stdlib/date_time")
local tp = dt.local_now()
local d = dt.seconds(1) + dt.minutes(2)
local b = dt.nanoseconds(2)
b += b
local dd = dt.nanoseconds(10)
print(`dd = {dd} {dd + b}`)
b += dd
print(`b = {b}`)
print(`{d}, {d.seconds}`)
print(`{tp.day}/{tp.month}/{tp.year} - {tp.hour}:{tp.minute}:{tp.second}`)
print(tp)

-- local window = desktop.window.create(
--     "hello", 30, 30, 800, 600,
--     bit32.bor(desktop.window.FLAGS.SHOWN,
--     desktop.window.FLAGS.RESIZABLE)
-- )
-- local quit = false
-- local event = desktop.event.create()

--print(`{cc.fg.red} yoo`)
--print(`window title: {window.title}`)
--print(`{cc.reset}frfr`)
-- window.title = "new title"
--
-- while not quit do
--     while desktop.poll_event(event) do
--         local type = desktop.EVENT_TYPE
--         if event.type == type.QUIT then
--             quit = true
--         end
--     end
-- end
