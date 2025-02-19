local definitions = {"lsp/dluau-api.d.luau", "lsp/dluau-std.d.luau"}
local docs = {"lsp/dluau-api-docs.json"}

require('lsp.nvimlspsetup').setup(definitions, docs)
