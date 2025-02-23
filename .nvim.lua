local definitions = {
    "lsp/dluau-core.d.luau",
    "lsp/dluau-std.d.luau",
}
local docs = {
    "lsp/dluau-core_docs.json",
    "lsp/dluau-std_docs.json",
}

require('lsp.nvimlspsetup').setup(definitions, docs)
