
local ee = require('@gpm/BufferArray')
local ESC = string.char(27)
return table.freeze{
    black = `{ESC}[40m`,
    red = `{ESC}[41m`,
    green = `{ESC}[42m`,
    yellow = `{ESC}[43m`,
    blue = `{ESC}[44m`,
    magenta = `{ESC}[45m`,
    cyan = `{ESC}[46m`,
    white = `{ESC}[47m`,
    default = `{ESC}[49m`,
    brightblack = `{ESC}[100m`,
    brightred = `{ESC}[101m`,
    brightgreen = `{ESC}[102m`,
    brightyellow = `{ESC}[103m`,
    brightblue = `{ESC}[104m`,
    brightmagenta = `{ESC}[105m`,
    brightcyan = `{ESC}[106m`,
    brightwhite = `{ESC}[107m`,
}
