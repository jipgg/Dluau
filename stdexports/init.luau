local std = {}
local filesystem = require("@std/filesystem")
std.filesystem = filesystem
export type filesystemPath = filesystem.Path

return std
