local now = os.clock()
local ex = os.execute("echo hello world > ech.txt")
print(`exit code is {ex}`)
assert(os.rename('ech.txt', 'echo.txt'))
assert(os.remove('echo.txt'))
assert(os.remove('echo.txt'))
local t = os.time{year=2000, month=10, day=10}
print(t)
print(os.difftime(os.clock(), now))
