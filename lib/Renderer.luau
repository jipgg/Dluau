local SDL = require("@halua/SDL2")

local Renderer = {
    ptr = nil :: opaque?
}
Renderer.__index = Renderer

function Renderer.new(window: opaque, flags: number)
    return setmetatable({
        ptr = SDL.CreateRenderer(window, -1, flags)
    }, Renderer)
end

function Renderer:setDrawColor(r: number?, g: number?, b: number?, a: number?)
    SDL.SetRenderDrawColor(self.ptr, r or 0, g or 0, b or 0, a or 0xff)
end
function Renderer:clear()
    SDL.RenderClear(self.ptr)
end
function Renderer:present()
    SDL.RenderPresent(self.ptr)
end
function Renderer:drawRect(rect: SDL.Rect)
    SDL.RenderDrawRect(self.ptr, rect)
end
function Renderer:fillRect(rect: SDL.Rect)
    SDL.RenderFillRect(self.ptr, rect)
end

function Renderer:__gc()
    print("yooo gc")
    if self.ptr then
        SDL.DestroyRenderer(self.ptr)
    end
end

return Renderer
