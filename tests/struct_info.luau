local dc = dlimport.cinterop
local ctype = dlimport.cinterop.native_type
local point_info = dc.create_struct_info(8, {
    ['x'] = {type = 'c_int', memory_offset = 0},
    ['y'] = {type = 'c_int', memory_offset = 4},
})
local buf = buffer.create(8)
dc.set_struct_field(point_info, buf, 'x', ctype.int(2))
print(dc.get_struct_field(point_info, buf, 'y'))
print(point_info)
