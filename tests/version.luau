local dluaulib_name = assert(dlimport.searchpath("dluaulib"))
--print(dluaulib_name)
local dluaulib = dlimport.load(dluaulib_name)
--print(dluaulib.path, dluaulib.name)
local version = {
    string = dluaulib:rawcbinding('string', 'get_version') :: () -> string,
    major = dluaulib:rawcbinding('c_int', 'get_version_major') :: () -> number,
    minor = dluaulib:rawcbinding('c_int', 'get_version_minor') :: () -> number,
    patch = dluaulib:rawcbinding('c_int', 'get_version_patch') :: () -> number,
}
local test = {
    unsigned_int_return = dluaulib:rawcbinding('c_uint', 'test_unsigned_int_return') :: () -> c_uint,
    unsigned_int_arg = dluaulib:rawcbinding('c_uint', 'test_unsigned_int_arg', 'c_uint') :: (e: c_uint) -> c_uint,
    int_return = dluaulib:rawcbinding('c_int', 'test_int_return') :: () -> c_int,
    unsigned_char_return = dluaulib:rawcbinding('c_uchar', 'test_unsigned_char_return') :: () -> c_uchar,
}
print(`{nameof(test.unsigned_int_return)}: {test.unsigned_int_return()}`)
print(`{nameof(test.unsigned_int_arg)}: {test.unsigned_int_arg(dlimport.c.uint(0xff))}`)
print(`{nameof(test.int_return)}: {test.int_return()}`)
print(`{nameof(test.unsigned_char_return)}: {test.unsigned_char_return()}`)

print(version.string())
print(`version is {version.major()}.{version.minor()}.{version.patch()}`)

local function describe<T>(val: T, name: string?): string
    local nm = if name then `({name}) ` else ''
    local str: string = `{nm}({typeof(val)}): \{\n`
    for i, v in val :: any do
        str ..= `   {i} ({typeof(i)}): {v} ({typeof(v)}),\n`
    end
    str ..= '}\n'
    return str
end
print(describe(dlimport.getmodules()))
local cc = require('console_codes')
print(`{cc.brightgreen}{script.name} tests successful{cc.reset}`)
