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
--- Standard Console Module
export type Console_Standard_Module = {
    --- ansi escape sequence ("\033")
    --- "\033[0m" to reset all
    ansi_code: (escape_sequence: string) -> (),
    reset_attributes: () -> (),
    clear: () -> (),
    codes: ansi_codes,
}

return dll.loadfunction("luminstd.dll", "module_console")() :: Console_Standard_Module 
