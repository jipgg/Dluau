local dc = cinterop
local native = dc.native
local struct = dc.struct
local dll = dlimport.load(assert(dlimport.searchpath("dluaulib")))
local get_version = dc.bindfunction(dll, 'string', 'get_version')
print(`version {get_version()}`)
local POINT_SI;
local POINT = {
    __index = function(self: userdata, key: string)
        return struct.getfield(POINT_SI, self, key)
    end,
    __newindex = function(self: userdata, key: string, val: any)
        if typeof(val) == 'number' then
            struct.setfield(POINT_SI, self, key, native.int(val))
        else
            struct.setfield(POINT_SI, self, key, val)
        end
    end,
    __type = "point",
    __tostring = function(self: userdata)
        local x: c_int = struct.getfield(POINT_SI, self, 'x');
        local y: c_int = struct.getfield(POINT_SI, self, 'y');
        return `point: \{{x}, {y}\}`
    end,
}
POINT_SI = struct.newinfo(8, {
    ['x'] = {type = 'c_int', memoffset = 0},
    ['y'] = {type = 'c_int', memoffset = 4},
}, POINT)
type point = {
    x: number | c_int,
    y: number | c_int,
}
local test_point: (x: c_int, y: c_int) -> point = dc.bindfunction(dll, POINT_SI, "test_point", 'c_int', 'c_int')
local pt = test_point(native.int(10), native.int(100))
print(pt)
print(pt.x, pt.y)
pt.y = 1000
print(pt)
local print_point: (buf: point) -> () = dc.bindfunction(dll, 'void', 'print_point', POINT_SI);
print_point(pt)
