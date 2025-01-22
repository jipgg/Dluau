local cio = require('@xtm/console')
local time = require('@xtm/time')
local cc = cio.codes
local ansi = cio.ansi
cio.write("input? ")
print(`{cc.fg.magenta} yooo`)
print(`{cio.ansi('[0m')}`)
local input = cio.read()
print(`input is: {input}`)
print("test2 done")
print(`{time.now()}`)
