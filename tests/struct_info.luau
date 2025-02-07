local ci = dlimport.cinterop
local point = ci.create_struct_info(8, {
    ['x'] = {
        type = 'c_int',
        memory_offset = 0,
    },
    ['y'] = {
        type = 'c_int',
        memory_offset = 4,
    }
})
print(point)
