local time = std.time
for v in std.fs.current_directory():each_child() do
    print(`{v.name} = {typeof(v)} = {v.path}`)
end
print(time.now())


local pwsh = std.process.search_path("powershell")
if pwsh then
    std.process.system(pwsh, 'echo', 'hello', 'world')
else
end

local ok, err = pcall(function()
end)
if not ok then
    print(err)
end
