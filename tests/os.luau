local winapi = std.os.windows
print(std.time.now())
print(std.time.minutes(10))
local e = require('console_codes')
print(e.blue, "here", e)
local my_child = std.process.child("echo yoooooooooooo")
my_child:join()
print(`{nameof(my_child)} exited with {my_child.exit_code}`)
local id = winapi.message_box("hello world", "yo",
    bit32.bor(winapi.MB_CANCELTRYCONTINUE)
)
if id == winapi.IDOK then
    print("ok")
elseif id == winapi.IDNO then
    print('no')
elseif id == winapi.IDCANCEL then
    print('cancel')
else
    print(`{nameof(id)} {id}`)
end
