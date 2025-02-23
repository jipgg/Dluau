local winapi = std.os.windows
local A = 0x41
local B = 0x42
local R = 0x52
local DOWN_MASK = 0x8000
local TOGGLE_MASK = 0x0001

while true do
    if bit32.band(winapi.get_async_key_state(A), 0x8000) ~= 0 then
        print("A is down")
    elseif bit32.band(winapi.get_async_key_state(R), 0x8000) ~= 0 then
        std.io.println('bye')
        std.os.exit(0)
    end
    std.os.sleep(1)
end
