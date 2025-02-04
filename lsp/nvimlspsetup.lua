-- nvim lsp setup file
local directory_aliases = {}
local command = {"luau-lsp", "lsp"}

local function read_file(path)
  local fd = vim.uv.fs_open(path, "r", 438)
  if not fd then
    error("Failed to open file: " .. path)
  end
  local stat = vim.uv.fs_fstat(fd)
  if not stat then
    error("Failed to get file stats: " .. path)
  end
  local data = vim.uv.fs_read(fd, stat.size, 0)
  vim.uv.fs_close(fd)
  if not data then
    error("Failed to read file: " .. path)
  end
  return data
end
local function parse_json(json_str)
  local ok, result = pcall(vim.json.decode, json_str)
  if not ok then
    error("Failed to parse JSON: " .. result)
  end
  return result
end
local function load_config(path)
    local read_success, content = pcall(read_file, path)
    if not read_success then return nil end
    local parse_success, parsed = pcall(parse_json, content)
    if not parse_success then return nil end
    return parsed
end

local cwd_conf = load_config(vim.uv.cwd().."/.luaurc")
if cwd_conf then
    for key, value in pairs(cwd_conf.aliases) do
        directory_aliases['@' .. key] = vim.fs.normalize(value)
    end
end

local M = {}
function M.setup(definitions, docs)
    for i, v in ipairs(definitions) do
        table.insert(command, "--definitions="..definitions[i])
    end
    for i, v in ipairs(docs) do
        table.insert(command, "--docs="..docs[i])
    end
    require("lspconfig").luau_lsp.setup {
        cmd = command,
        settings = {
            ["luau-lsp"] = {
                platform = {
                    type = "standard",
                },
                require = {
                    mode = "relativeToFile",
                    directoryAliases = directory_aliases,
                },
            }
        }
    }
end

return M
