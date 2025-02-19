local winapi = std.os.windows.winapi
print(std.time.now())
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
-- print(result, os.platform)
-- print(os.windows.get_module_handle())
-- local now = os.clock()
-- local ex = os.execute("echo hello world > ech.txt")
-- print(`exit code is {ex}`)
-- local t = os.time{year=2000, month=10, day=10}
-- print(t)
-- print(os.difftime(os.clock(), now))
