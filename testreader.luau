local file = fs.File.open('./test.luau')

for i, v in process.args do
    print(`[{i} arg] = {v}`)
end

for line in file:reader():line_iterator() do
    print(line)
end
