local console = require('@xtm/console')
local time = require('@xtm/time')
local cc = console.codes
local ansi = console.ansi
console.write("input? ")
print(`{cc.fg.magenta} yooo`)
print(`{ansi('[0m')}`)
print("test2 done")
print(`{time.now()}`)
