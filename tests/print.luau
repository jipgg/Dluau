print(std)
local cc = require('console_codes')
print.separator = '\t'
print.noline(`{cc.cyan}yo input?: {cc.default}`)
local input = tonumber(scan())
print(`input is {input}`)
