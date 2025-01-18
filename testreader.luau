local file = fs.File.open('./test.luau')

for i, v in process.args do
    print(`arg[{i}]: {v}`)
end

