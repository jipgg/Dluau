local dc = dlimport.cinterop
local nt = dlimport.cinterop.native_type
local dll = dlimport.load(assert(dlimport.searchpath("dluaulib")))
local get_version = dc.new_function_binding(dll, 'string', 'get_version')
print(`version {get_version()}`)
local POINT_INFO = dc.create_struct_info(8, {
    ['x'] = {type = 'c_int', memory_offset = 0},
    ['y'] = {type = 'c_int', memory_offset = 4},
})
local buf = buffer.create(8)
dc.set_struct_field(POINT_INFO, buf, 'x', nt.int(2))
print(dc.get_struct_field(POINT_INFO, buf, 'y'))
print(POINT_INFO)
local test_point: (x: c_int, y: c_int) -> buffer = dc.new_function_binding(dll, POINT_INFO, "test_point", 'c_int', 'c_int')
local ptbuf = test_point(nt.int(10), nt.int(100))
print(buffer.readi32(ptbuf, 4))
print(dc.get_struct_field(POINT_INFO, ptbuf, 'x'))
local print_point: (buf: buffer) -> () = dc.new_function_binding(dll, 'void', 'print_point', POINT_INFO);
dc.set_struct_field(POINT_INFO, ptbuf, 'y', nt.int(200))
print_point(ptbuf)
