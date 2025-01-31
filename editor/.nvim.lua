-- rough remote config file for luau-lsp support for dluau in neovim
local DLUAU_ROOT = os.getenv("DLUAU_ROOT")
local ROOT_REQUIRE_DIR = DLUAU_ROOT.."/require"
local ROOT_CONFIG = os.getenv("DLUAU_ROOT").."/.dluaurc.json"
local directory_aliases = {}
local uv = vim.uv

local function load_root_dirs(path)
    local dir = uv.fs_opendir(path, nil)
    if not dir then
        error("Failed to open directory: " .. path)
    end
    local entries = uv.fs_readdir(dir)
    while entries do
        for _, entry in ipairs(entries) do
            local entry_path = path .. "/" .. entry.name
            if entry.type == "directory" then
                directory_aliases['@' .. entry.name] = entry_path
            end
        end
        entries = uv.fs_readdir(dir)
    end
    uv.fs_closedir(dir)
end

local function read_file(path)
  local fd = uv.fs_open(path, "r", 438)
  if not fd then
    error("Failed to open file: " .. path)
  end
  local stat = uv.fs_fstat(fd)
  if not stat then
    error("Failed to get file stats: " .. path)
  end
  local data = uv.fs_read(fd, stat.size, 0)
  uv.fs_close(fd)
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

load_root_dirs(ROOT_REQUIRE_DIR)
local root_conf = load_config(ROOT_CONFIG)
if root_conf then
    for key, value in pairs(root_conf.aliases) do
        directory_aliases['@' .. key] = vim.fs.normalize(DLUAU_ROOT.."/"..value)
    end
end
local cwd_conf = load_config(vim.uv.cwd().."/.luaurc") or load_config(uv.cwd().."/.dluaurc.json")
if cwd_conf then
    for key, value in pairs(cwd_conf.aliases) do
        if string.find(value, '$') then
            local ending = string.find(value, "/")
            if ending then
                local after = string.sub(value, ending)
                value = os.getenv(string.sub(value, 2, ending - 1)) .. after
            else
                value = os.getenv(string.sub(value, 2))
            end
        end
        directory_aliases['@' .. key] = vim.fs.normalize(""..value)
    end
end

require"lspconfig".luau_lsp.setup {
    cmd = {"luau-lsp",
        "lsp",
        "--definitions=dluau_definitions",
        "--docs=dluau_docs.json",
    },
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
