local console = require("@minluau/console")
local desktop = require("@minluau/desktop")
local cc = console.codes

local window = desktop.window.create(
    "hello", 30, 30, 800, 600,
    bit32.bor(desktop.window.FLAGS.SHOWN,
    desktop.window.FLAGS.RESIZABLE)
)
local quit = false
local event = desktop.event.create()

print(`{cc.fg.red} yoo`)
print(`window title: {window.title}`)
print(`{cc.reset}frfr`)
window.title = "new title"

while not quit do
    while desktop.poll_event(event) do
        local type = desktop.EVENT_TYPE
        if event.type == type.QUIT then
            quit = true
        end
    end
end
