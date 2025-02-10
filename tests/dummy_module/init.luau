local module = {}
module.name = script.name
module.directory = script.directory
type submodule = typeof(require('submodule'))
module.submodule = lazyrequire('submodule') :: submodule
return module
