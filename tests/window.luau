local winapi = std.os.api.windows
local A = 0x41
local B = 0x42
local R = 0x52
local DOWN_MASK = 0x8000
local TOGGLE_MASK = 0x0001
local messagebox = std.os.messagebox
if messagebox('abc', 'abc', 'yes no cancel') == 'yes' then
    print('yes')
end

while true do
    if std.os.keydown('a') then
        print("A is down")
    elseif std.os.keydown('esc') then
        std.io.println('bye')
        std.os.exit(0)
    end
    std.os.sleep(1)
end
