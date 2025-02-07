local dc = dlimport.cinterop
local native = dc.native
local struct = dc.struct
local dll = dlimport.load(assert(dlimport.searchpath("dluaulib")))
local get_version = dc.bindfunction(dll, 'string', 'get_version')
print(`version {get_version()}`)
local POINT_INFO;
local POINT_META = {
    __index = function(self: userdata, key: string)
        return struct.getfield(POINT_INFO, self, key)
    end,
    __newindex = function(self: userdata, key: string, val: any)
        if typeof(val) == 'number' then
            struct.setfield(POINT_INFO, self, key, native.int(val))
        else
            struct.setfield(POINT_INFO, self, key, val)
        end
    end,
    __type = "point",
    __tostring = function(self: userdata)
        local x: c_int = struct.getfield(POINT_INFO, self, 'x');
        local y: c_int = struct.getfield(POINT_INFO, self, 'y');
        return `point: \{{x}, {y}\}`
    end,
}
POINT_INFO = struct.newinfo(8, {
    ['x'] = {type = 'c_int', memoffset = 0},
    ['y'] = {type = 'c_int', memoffset = 4},
}, POINT_META)
type point = {
    x: number | c_int,
    y: number | c_int,
}
local buf = buffer.create(8)
struct.setfield(POINT_INFO, buf, 'x', native.int(20))
print(struct.getfield(POINT_INFO, buf, 'y'))
print(POINT_INFO)
local test_point: (x: c_int, y: c_int) -> point = dc.bindfunction(dll, POINT_INFO, "test_point", 'c_int', 'c_int')
local ptbuf = test_point(native.int(10), native.int(100))
print(ptbuf)
print(ptbuf.x, ptbuf.y)
ptbuf.y = 1000
print(ptbuf)
-- print(struct.getfield(POINT_INFO, ptbuf, 'x'))
-- local print_point: (buf: buffer) -> () = dc.bindfunction(dll, 'void', 'print_point', POINT_INFO);
-- struct.setfield(POINT_INFO, ptbuf, 'y', native.int(200))
-- print_point(ptbuf)

