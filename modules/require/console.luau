export type ansi_color_codes = {
    default: string,
    black: string,
    red: string,
    green: string,
    yellow: string,
    blue: string,
    magenta: string,
    cyan: string,
    white: string,
    gray: string,
    light_red: string,
    light_green: string,
    light_yellow: string,
    light_blue: string,
    light_magenta: string,
    light_cyan: string,
    bright_white: string,
}
export type ansi_codes = {
    reset: string,
    fg: ansi_color_codes,
    bg: ansi_color_codes,
}
export type console = {
    read: () -> string,
    write: (msg: string) -> (),
    errwrite: (msg: string) -> (),
    --- ansi escape sequence ("\033")
    --- "\033[0m" to reset all
    ansi: (escape_sequence: string) -> (),
    clear: () -> (),
    codes: ansi_codes,
    cursor: {
        move_up: () -> (),
        move_down: () -> (),
        move_right: () -> (),
        move_left: () -> (),
    }
}
local dyn = dynlib.load("xtm_console")
local binding_test: (a: number, b: number) -> number
    = dyn:create_binding('int', 'add_numbers', 'double', 'double')
print(`{binding_test}`)
print(binding_test(100, 100))
print(`result is {binding_test(120, 130)}`)

return dyn:lua_cfunction("module_console") () :: console 
