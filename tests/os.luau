local dll = os.windows.load_library('dluaulib')
if dll then
    task.defer(os.windows.free_library, dll)
end
print(dll)
local result = os.windows.message_box("hello world", "YOO")
-- print(result, os.platform)
-- print(os.windows.get_module_handle())
-- local now = os.clock()
-- local ex = os.execute("echo hello world > ech.txt")
-- print(`exit code is {ex}`)
-- local t = os.time{year=2000, month=10, day=10}
-- print(t)
-- print(os.difftime(os.clock(), now))
