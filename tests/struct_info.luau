local dc = dlimport.cinterop
local native = dc.native
local struct = dc.struct
local dll = dlimport.load(assert(dlimport.searchpath("dluaulib")))
local get_version = dc.bindfunction(dll, 'string', 'get_version')
print(`version {get_version()}`)
local POINT_INFO = struct.newinfo(8, {
    ['x'] = {type = 'c_int', memory_offset = 0},
    ['y'] = {type = 'c_int', memory_offset = 4},
})
local buf = buffer.create(8)
struct.setfield(POINT_INFO, buf, 'x', native.int(20))
print(struct.getfield(POINT_INFO, buf, 'y'))
print(POINT_INFO)
local test_point: (x: c_int, y: c_int) -> buffer = dc.bindfunction(dll, POINT_INFO, "test_point", 'c_int', 'c_int')
local ptbuf = test_point(native.int(10), native.int(100))
print(buffer.readi32(ptbuf, 4))
print(struct.getfield(POINT_INFO, ptbuf, 'x'))
local print_point: (buf: buffer) -> () = dc.bindfunction(dll, 'void', 'print_point', POINT_INFO);
struct.setfield(POINT_INFO, ptbuf, 'y', native.int(200))
print_point(ptbuf)
