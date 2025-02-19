print(std.cinterop.struct_info)
local dls = dlimport.getmodules()
local cinterop_dl: dlmodule
for i, v in dls do
    if v.name == "dluau_std_cinterop" then
        cinterop_dl = v
    end
end
assert(cinterop_dl, "failed to find dl")
local print_test_struct_info: () -> () = std.cinterop.bind_function(
    cinterop_dl, 'void', "print_test_struct_info"
)
--print_test_struct_info()
local Test_struct: struct_info
local Test_struct_metatable = {
    __index = function(ud: opaque, key: string)
        if (key == "some_char_arr") then
            return setmetatable({}, {
                __index = function(_, idx)
                    return Test_struct:get_field(ud, key, idx)
                end,
                __newindex = function(_, idx, val)
                    Test_struct:set_field(ud, key, val, idx)
                end
            })
        else
            return Test_struct:get_field(ud, key)
        end
    end,
    __newindex = function(ud: opaque, key: string, val: number)
        if (key == "some_char_arr") then
            error("cant set field some_char_arr")
        end
        return Test_struct:set_field(ud, key, val)
    end,
    __type = `local.{nameof(Test_struct)}`,
}
Test_struct = std.cinterop.struct_info(24, {
    some_int = {
        type = 'int',
        memory_offset = 0,
    },
    some_float = {
        type = 'float',
        memory_offset = 4,
    },
    some_unsigned_int = {
        type = 'uint',
        memory_offset = 8,
    },
    some_char = {
        type = 'char',
        memory_offset = 12,
    },
    some_char_arr = {
        type = 'char',
        memory_offset = 13,
        array_size = 10,
    }
}, Test_struct_metatable)
type Test_struct = {
    some_int: number,
    some_float: number,
    some_unsigned_int: number,
    some_char: number,
    some_char_arr: {number},
}
local print_struct_by_value: (Test_struct) -> () = std.cinterop.bind_function(
    cinterop_dl, 'void', "print_struct_by_value", Test_struct
)

-- local print_struct_by_pointer: (userdata) -> () = std.cinterop.bind_function(
--     cinterop_dl, 'void', "print_struct_by_pointer", 'void*'
-- )
local test_obj: Test_struct = Test_struct:new_instance()
test_obj.some_int = -10
test_obj.some_unsigned_int = 0
test_obj.some_float = 1.23
test_obj.some_char_arr[1] = 97
print(test_obj.some_char_arr[1])
print(test_obj)
print_struct_by_value(test_obj)
print(test_obj.some_int)
print(test_obj.some_unsigned_int)


-- local point: struct_info
-- local point_metatable = {
--     __index = function(ud: opaque, key: string)
--         return point:get_field(ud, key) :: number
--     end,
--     __newindex = function(ud: opaque, key: string, val: number)
--         return point:set_field(ud, key, val)
--     end,
--     __tostring = function(ud: opaque)
--         return `{(ud::any).x}, {(ud::any).y}`
--     end,
-- }
-- type point = typeof(setmetatable({x = 1, y = 1}, point_metatable))
-- point = std.cinterop.struct_info(8, {
--     x = {type = 'int', memory_offset = 0},
--     y = {type = 'int', memory_offset = 4}
--     }, point_metatable)
-- local p: point = point:new_instance()
-- p.y = 10
-- print(`({p.x}, {p.y})`)
-- print(p.x, p.y)
