local mod = require('./mod')
print(`{meta.current_script}: {nameof(mod)}`)
local my_variable = 1

local function abc()
    local some_data = "hellllooo"
    print(`data is "{nameof(some_data)}"`)
    return 123
end
print(`variable is "{nameof(my_variable)}"`)
print(`aaa {nameof(vector)}`)
print(`aaa {nameof(vector.zero)}`)
