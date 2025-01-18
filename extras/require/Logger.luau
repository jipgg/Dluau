local class = {}
local Logger = {
}
local time = require('@extras/time')
Logger.__index = Logger

function class.wrap(file: fsFile)
    return setmetatable({
        file_ = file,
        writer_ = nil,
        written = 0,
    }, Logger)
end

function Logger:log<Ts...>(...: Ts...)
    local file: fsFile = self.file_
    local writer: ioWriter? = self.writer_
    if not writer then
        writer = file:writer(true)
    end
    local packed = table.pack(...)
    packed['n'] = nil
    (writer :: ioWriter):print(`[{time.now()}] {table.concat(packed, ", ")}`)
end

export type Log_entry = {
    timestamp: string,
    message: string,
}
function Logger:iterator()
    local dummy: Log_entry = {
        timestamp = "",
        message = "",
    }
    local reader = (self.file_:reader()) :: ioReader
    local lit = reader:line_iterator()
    return function(): Log_entry?
        local next = lit()
        if (not next) then return nil end
        local first = string.find(next, "]") :: number
        dummy.timestamp = string.sub(next, 2, first - 1)
        dummy.message = string.sub(next, first + 2)
        return dummy
    end
end


export type Logger = typeof(setmetatable({
}, Logger))

export type Module = {
    wrap: (file: fsFile) -> Logger,
}

print(`LOGGER SCRIPT PATH: {script.file}`)
return class :: Module
