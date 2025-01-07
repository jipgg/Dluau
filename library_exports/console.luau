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
export type console_library = {
    set_foreground_color: (color: number) -> (),
    set_background_color: (color: number) -> (),
    --- ansi escape sequence ("\033")
    --- "\033[0m" to reset all
    ansi: (escape_sequence: string) -> (),
    read: () -> string,
    Color: Color,
}

return cimport("halua-lib.dll", "console_library")() :: console_library
