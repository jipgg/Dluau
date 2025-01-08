local console = require("@minluau/console")
local desktop = require("@minluau/desktop")

local window = desktop.window.create(
    "hello", 30, 30, 800, 600,
    bit32.bor(desktop.window.FLAGS.SHOWN,
    desktop.window.FLAGS.RESIZABLE)
)
local quit = false
local event = desktop.event.create()
console.set_foreground_color(console.Color.Magenta)
print(`window title: {window.title}`)
window.title = "new title"
console.set_foreground_color(console.Color.Default)

while not quit do
    while desktop.poll_event(event) do
        if event.type == desktop.EVENT_TYPE.QUIT then
            quit = true
        end
    end
end
