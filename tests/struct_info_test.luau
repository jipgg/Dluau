local any: any
local str: string
local point: struct_info
local point_metatable = {
    __index = function(ud: userdata, key: string)
        return point:get_field(ud, key) :: number
    end,
    __newindex = function(ud: userdata, key: string, val: number)
        return point:set_field(ud, key, val)
    end,
    __tostring = function(ud: userdata)
        return `{ud.x}, {ud.y}`
    end,
}
type point = typeof(setmetatable({x = 1, y = 1}, point_metatable))
point = std.cinterop.struct_info(8, {
    x = {type = 'int', memory_offset = 0},
    y = {type = 'int', memory_offset = 4}
    }, point_metatable)
local p: point = point:new_instance()
p.y = 10
print(p.x, p.y)
