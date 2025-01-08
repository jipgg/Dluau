local sdl = require("@halua/SDL2")
sdl.Init()
local window = sdl.window
local my_window = window.create(
    "hello", window.UNDEFINED,
    window.UNDEFINED, 800, 600,
    bit32.bor(window.flags.SHOWN,
        window.flags.RESIZABLE)
)
local event = sdl.Event()
local quit = false

my_window.opacity = 1
while not quit do
    while sdl.PollEvent(event) do
        if event.type == sdl.EventType.QUIT then
            quit = true
        end
    end
end

