require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=lumin-core.d.luau",
        "--definitions=modules/require/filesystem/defs.luau",
    },
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@xtm"] = "./modules/require/",
                },
            },
        }
    }
}
