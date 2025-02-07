type field = {
    name: string,
    type: c_type,
    memory_offset: number?,
    array_size: number?,
}
type aggr = (fields: {field}) -> c_aggregate

local struct_description = (nil :: any) :: aggr;

local function field(name: string, type: c_type, memory_offset: number?, array_size: number?): field
    return {name = name, type = type, memory_offset = memory_offset, array_size = array_size}
end

type Point = {
    x: c_int,
    y: c_int,
}
local point_aggr = struct_description {field('x', 'c_int'), field('y', 'c_int')}
