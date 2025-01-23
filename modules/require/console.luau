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
    clear: () -> (),
    --- ansi escape sequence ("\033")
    --- "\033[0m" to reset all
    ansi: (escape_sequence: string) -> (),
    codes: ansi_codes,
    cursor: {
        move_up: () -> (),
        move_down: () -> (),
        move_right: () -> (),
        move_left: () -> (),
    }
}
return  dlimport.load("xtm_console"):lua_cfunction("luauxtm_load")() :: console 
