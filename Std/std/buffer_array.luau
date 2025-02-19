local _Types = require('types')
local _I8 = 1
local _I16 = 2
local _I32 = 3
local _U8 =  4
local _U16 = 5
local _U32 = 6
local _F32 = 7
local _F64 = 8
local function _To_literal(t: number): _Types.Literal
    if t == _I8 then return 'i8'
    elseif t == _I16 then return 'i16'
    elseif t == _I32 then return 'i32'
    elseif t == _U8 then return 'u8'
    elseif t == _U16 then return 'u16'
    elseif t == _U32 then return 'u32'
    elseif t == _F32 then return 'f32'
    elseif t == _F64 then return 'f64'
    end
    error("")
end
local _Read = {
    buffer.readi8,
    buffer.readi16,
    buffer.readi32,
    buffer.readu8,
    buffer.readu16,
    buffer.readu32,
    buffer.readf32,
    buffer.readf64,
}
local _Write = {
    buffer.writei8,
    buffer.writei16,
    buffer.writei32,
    buffer.writeu8,
    buffer.writeu16,
    buffer.writeu32,
    buffer.writef32,
    buffer.writef64,
}
local function _Init_buffer(this: _Types._Buffer_array_type<unknown>, ...): _Types._Buffer_array_type<unknown>
    local _Tuple_size = select('#', ...)
    if _Tuple_size == 1 then
        local size_or_arr = select(1, ...)
        if typeof(size_or_arr) == 'number' then
            this._Data = buffer.create(size_or_arr * this._Width)
        elseif typeof(size_or_arr) == 'table' then
            local size = #size_or_arr
            this._Data = buffer.create(size * this._Width)
            for i, v in size_or_arr do
                _Write[this._Ty](this._Data, (i - 1) * this._Width, v)
            end
        end
    else
        this._Data = buffer.create(_Tuple_size * this._Width)
        for i = 1, _Tuple_size do
            _Write[this._Ty](
                this._Data,
                (i - 1) * this._Width,
                select(i, ...)
            )
        end
    end
    return this
end

local _Meta: _Types._Impl_buffer_array_type = {
    type = "BufferArray",
    full_type = "gpm.BufferArray",
} :: any
function _Meta:read(idx: number)
    return _Read[self._Ty](self._Data, self._Width * idx)
end
function _Meta:write(idx: number, val: number)
    _Write[self._Ty](self._Data, self._Width * idx, val)
end
function _Meta:__index(idx)
    if typeof(idx) == 'string' then
        return _Meta[idx]
    elseif typeof(idx) == 'number' then
        return self:read(idx)
    end
    error(`type of '{nameof(idx)}' is invalid '{typeof(idx)}'`)
end
function _Meta:__newindex(idx, value)
    return self:write(idx, value)
end
function _Meta:fill(value: number, start_index: number?, count: number?)
    local offset = (start_index or 0) * self._Width
    local size = if count then count * self._Width else nil
    buffer.fill(self._Data, offset, value, size)
end
function _Meta:__tostring()
    local t: {number} = table.create(#self)
    for i, v in self:pairs() do
        table.insert(t, v)
    end
    return `<{_To_literal(self._Ty)}>\{{table.concat(t, ',')}\}`
end
function _Meta:pairs()
    local idx = 0
    local count = #self
    return function()
        if idx < count then
            local i, v = idx, self:read(idx) 
            idx += 1
            return i, v
        else
            return nil, nil :: any
        end
    end
end
function _Meta:reverse_pairs()
    local idx = #self - 1
    return function()
        if idx >= 0 then
            local i, v = idx, self:read(idx) 
            idx -= 1
            return i, v
        else
            return nil, nil :: any
        end
    end
end
function _Meta:__len()
    return buffer.len(self._Data) // self._Width
end

local _Module: any = {}
function _Module.I8(...)
    return setmetatable(_Init_buffer({
        _Ty = _I8,
        _Width = 1,
    } :: any, ...), _Meta)
end
function _Module.I16(...)
    return setmetatable(_Init_buffer({
        _Ty = _I16,
        _Width = 2,
    } :: any, ...), _Meta)
end
function _Module.I32(...)
    return setmetatable(_Init_buffer({
        _Ty = _I32,
        _Width = 4,
    } :: any, ...), _Meta)
end
function _Module.U8(...)
    return setmetatable(_Init_buffer({
        _Ty = _U8,
        _Width = 1,
    } :: any, ...), _Meta)
end
function _Module.U16(...)
    return setmetatable(_Init_buffer({
        _Ty = _U16,
        _Width = 2,
    } :: any, ...), _Meta)
end
function _Module.U32(...)
    return setmetatable(_Init_buffer({
        _Ty = _U32,
        _Width = 4,
    } :: any, ...), _Meta)
end
function _Module.F32(...)
    return setmetatable(_Init_buffer({
        _Ty = _F32,
        _Width = 4,
    } :: any, ...), _Meta)
end
function _Module.F64(...)
    return setmetatable(_Init_buffer({
        _Ty = _F64,
        _Width = 8,
    } :: any, ...), _Meta)
end
return _Module :: _Types.Module
