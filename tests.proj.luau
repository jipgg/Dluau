local project: Project_configuration = {
    sources = {
        "test2.luau",
        "test3.luau"
    },
    optimization_level = 0,
    debug_level = 1,
    aliases = {
        extras = "./extras/require/",
    },
}
return project
