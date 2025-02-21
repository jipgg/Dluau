
local some_invalid_json = "adaKDawl;kaak;fkaA"
local some_json = [[
{
    "languageMode": "strict",
    "aliases": {
        "time": "~/.dluau/require/time/",
        "guitest": "~/.dluau/require/guitest/",
        "gpm": "~/.dluau/require/gpm/"
    }
}
]]
local function describe(tbl: any): string
    local str: string = "{\n"
    for k, v in tbl do
        if typeof(v) == 'table' then
            str ..= `   [{k}]: {describe(v)}`
        end
        str ..= `   [{k}]: {v}\n`
    end
    str ..= "}"
    return str
end
local a = std.json.parse(some_json)
print(describe(a))
local b = std.json.parse(some_invalid_json)
print(b)
