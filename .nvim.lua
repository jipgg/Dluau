require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=minluau.definitions.luau",
    },
    settings = {
        ["luau-lsp"] = {
            platform = {
                type = "standard",
            },
            require = {
                mode = "relativeToFile",
                directoryAliases = {
                    ["@minluau"] = "./require/",
                },
            },
        }
    }
}
