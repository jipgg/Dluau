local module = require('dummy_module')
assert(module.name == 'init')
local submodule = require('dummy_module/submodule')
assert(submodule.name == 'submodule')
print.text_color = 'green'
print(`{script.name} tests successful`)
print.reset_colors()
