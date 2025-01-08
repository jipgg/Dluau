export type ColorItem = number
export type Color = {
    Default: number,
    Black: number,
    Red: number,
    Green: number,
    Yellow: number,
    Blue: number,
    Magenta: number,
    Cyan: number,
    LightGray: number,
    DarkGray: number,
    LightRed: number,
    LightGreen: number,
    LightYellow: number,
    LightBlue: number,
    LightMagenta: number,
    LightCyan: number,
    White: number,
}
export type console_color = {
    default: () -> opaque,
    black: () -> opaque,
    req: () -> opaque,
    green: () -> opaque,
    yellow: () -> opaque,
    blue: () -> opaque,
    magenta: () -> opaque,
    cyan: () -> opaque,
    light_gray: () -> opaque,
    gray: () -> opaque,
    light_red: () -> opaque,
    light_green: () -> opaque,
    light_blue: () -> opaque,
    light_magenta: () -> opaque,
    white: () -> opaque,
}
export type standard_module = {
    set_foreground_color: (color: number) -> (),
    set_background_color: (color: number) -> (),
    --- ansi escape sequence ("\033")
    --- "\033[0m" to reset all
    ansi: (escape_sequence: string) -> (),
    read: () -> string,
    color: console_color,
    Color: Color,
}

return dllimport("minluau.modules.dll", "import_console")() :: standard_module
