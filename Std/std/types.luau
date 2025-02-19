export type I8 = 'i8'
export type I16 = 'i16'
export type I32 = 'i32'
export type U8 = 'u8'
export type U16 = 'u16'
export type U32 = 'u32'
export type F32 = 'f32'
export type F64 = 'f64'
export type Literal = I8 | I16 | I32 | U8 | U16 | U32 | F32 | F64
export type BufferArrayType<T> = typeof(setmetatable(
    {} :: _Buffer_array_type<T>,
    {} :: _Meta_buffer_array_type
))
export type _Buffer_array_type<T> = {
    [number]: number,
    ['@T']: T,
    _Ty: number,
    _Width: number,
    _Data: buffer,
    type: "BufferArray",
    data: (self: BufferArrayType<T>) -> buffer,
    width: (self: BufferArrayType<T>) -> number,
    full_type: "gpm.BufferArray",
    read: (self: BufferArrayType<T>, idx: number) -> number,
    write: (self: BufferArrayType<T>, idx: number, val: number) -> (),
    pairs: (self: BufferArrayType<T>) -> (() -> (number?, number)),
    fill: (self: BufferArrayType<T>, value: number, start_index: number?, count: number?) -> (),
    reverse_pairs: (self: BufferArrayType<T>) -> (() -> (number?, number)),
}
export type _Meta_buffer_array_type = {
    __index: ((self: BufferArrayType<unknown>, index: number) -> number) & any,
    __newindex: (self: BufferArrayType<unknown>, index: number, value: number) -> (),
    __len: (self: BufferArrayType<unknown>) -> number,
    __tostring: (self: BufferArrayType<unknown>) -> string,
}

export type _Impl_buffer_array_type<T = unknown> = _Meta_buffer_array_type
    & _Buffer_array_type<T>
export type _New_buffer_array_type<T> = ((size: number) -> BufferArrayType<T>)
    & ((...number)->BufferArrayType<T>)
    & ((t: {number}) -> BufferArrayType<T>)

--- Abstraction around the luau standard `buffer` library for fixed type buffers.
--- Implemented in luau.
export type Module = {
    I8: _New_buffer_array_type<I8>,
    I16: _New_buffer_array_type<I16>,
    I32: _New_buffer_array_type<I32>,
    U8: _New_buffer_array_type<U8>,
    U16: _New_buffer_array_type<U16>,
    U32: _New_buffer_array_type<U32>,
    F32: _New_buffer_array_type<F32>,
    F64: _New_buffer_array_type<F64>,
}
return true
