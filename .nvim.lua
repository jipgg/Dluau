require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=LuauXT.d.luau",
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
