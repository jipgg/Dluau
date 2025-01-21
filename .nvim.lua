require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=lumin-core.d.luau",
        "--definitions=modules/require/fs/defs.luau",
    },
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@modules"] = "./modules/require/",
                },
            },
        }
    }
}
