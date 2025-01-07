local SDL = require("@halua/SDL2")
SDL.Init()
local window = SDL.Window("hellobro", SDL.WINDOWPOS_CENTERED, SDL.WINDOWPOS_CENTERED, 800, 600, bit32.bor(SDL.WindowFlags.SHOWN))
local event = SDL.Event()
local quit = false

window:setOpacity(0.5)
while not quit do
    while SDL.PollEvent(event) do
        if event.type == SDL.EventType.QUIT then
            quit = true
        end
    end
end

