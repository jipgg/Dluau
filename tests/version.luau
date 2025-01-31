local dluaulib_name = assert(dlimport.search_path("dluaulib"))
print(dluaulib_name)
local dluaulib = dlimport.load(dluaulib_name)
print(dluaulib.path, dluaulib.name)
local version = {
    string = dluaulib:create_c_binding('string', 'get_version') :: () -> string,
    major = dluaulib:create_c_binding('int', 'get_version_major') :: () -> number,
    minor = dluaulib:create_c_binding('int', 'get_version_minor') :: () -> number,
    patch = dluaulib:create_c_binding('int', 'get_version_patch') :: () -> number,
}
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
print(describe(dlimport.loaded_modules()))
print.text_color = 'green'
print(`{script.name} tests successful`)
print.reset_colors()
