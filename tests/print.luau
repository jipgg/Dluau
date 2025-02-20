print(std)
local blue = std.io.escape_code'[34m'
local reset_colors = std.io.escape_code'[0m'
std.io.println(`{blue} blueing`)

std.io.print("please give a number: ")
local parsed = tonumber(std.io.scan())
while not parsed do
    std.io.print("not a number, try again: ")
    parsed = tonumber(std.io.scan())
end
std.io.println(`the number is {parsed}{reset_colors}`)

local buf = buffer.create(parsed :: number)
std.io.print(`reading {parsed} characters: `); std.io.read(buf)
std.io.print('the written characters are: '); std.io.write(buf)
std.io.print('\n')

