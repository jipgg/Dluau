require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=dluau.d.luau",
    },
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@root"] = "/Users/jip/.dluau/require/",
                    ["@dlm"] = "./modules/require/",
                },
            },
        }
    }
}
