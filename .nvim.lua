require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=dluau_definitions",
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
                    ["@require"] = "/Users/jip/.dluau/require/",
                },
            },
        }
    }
}
