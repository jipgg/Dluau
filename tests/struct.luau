local struct = dlimport.c.struct
local sizeof = dlimport.c.sizeof
local dll = dlimport.load(assert(dlimport.searchpath("dluaulib")))
local print_test_info = dlimport.c.bindfunction(dll, 'void', 'print_test_info')
local TEST_SI
export type Test = {
    a: c_int,
    abc: string,
    yes: boolean,
    flt: c_float,
    num: number,
    uch: c_uchar,
}
local Test = {
    __type = "Test",
}
function Test.new(): Test
    return struct.newinstance(TEST_SI)
end
function Test:__index(name: string, arrindex: number?)
    if (name == 'abc') then
        local function tochar(index: number)
            return string.char(
                tonumber(
                    tostring(struct.getfield(TEST_SI, self, name, index)))
                :: number
            )
        end
        local str: string = ''
        str ..= tochar(0)
        str ..= tochar(1)
        str ..= tochar(2)
        return str
    end
    return struct.getfield(TEST_SI, self, name, arrindex)
end
function Test:__newindex(name: string, val: unknown, arrindex: number?)
    if (name == 'abc') then
        if typeof(val) == 'string' then
            local char = dlimport.c.native.char
            struct.setfield(TEST_SI, self, name, char(string.sub(val, 1, 2)), 0)
            struct.setfield(TEST_SI, self, name, char(string.sub(val, 2, 3)), 1)
            struct.setfield(TEST_SI, self, name, char(string.sub(val, 3, 4)), 2)
            return
        end
    end
    struct.setfield(TEST_SI, self, name, val, arrindex)
end
TEST_SI = struct.newinfo(32, {
    ['a'] = {
        type = 'c_int',
        memoffset = 0,
    },
    ['abc'] = {
        type = 'c_char',
        memoffset = 4,
        arrsize = 3,
    },
    ['yes'] = {
        type = 'boolean',
        memoffset = 7,
    },
    ['flt'] = {
        type = 'c_float',
        memoffset = 8,
    },
    ['num'] = {
        type = 'number',
        memoffset = 16,
    },
    ['uch'] = {
        type = 'c_uchar',
        memoffset = 24,
    }
}, Test)
local test = Test.new()
print(test.abc)
test.abc = 'abc'
print(test.abc)
test.num = 0.1032
test.flt += 190
test.flt -= 5.134
print(`size of long: {sizeof('c_long')}`)
print(`size of int: {sizeof('c_int')}`)
print(`size of pointer: {sizeof('userdata')}`)
test.flt *= 2.1
print(test.flt)
print(test.num)
print_test_info()
