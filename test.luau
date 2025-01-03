local fs = require("./stdexports/filesystem")
print("yo");
for i, v in fs.children("std") do
    print(`{i} = {v}`)
end
for i, v in fs.descendants("std") do
    print(`{i} = {v}`)
end
print("hello")
