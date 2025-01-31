local ESC = string.char(27)
local function merge<T, U>(t: T, u: U): T & U
    local ret = {}
    for i, v in t :: any do ret[i] = v end
    for i, v in u :: any do ret[i] = v end
    return ret :: any
end
local fg = require('fg')
local M = {
    bg = require('bg'),
    reset = `{ESC}[0m`
}

return merge(M, fg)
