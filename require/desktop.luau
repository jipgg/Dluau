export type rect = {x: number, y: number, w: number, h: number}
export type rect_struct = {
    create: (x: number?, y: number?, w: number?, h: number?) -> rect,
}
export type event = {type: number}
export type point = {x: number, y: number}
export type event_struct = {create: () -> event}
export type window = {
    opacity: number,
    title: string,
        --- getter
    position: ((self: window) -> (number, number))
        --- setter
        & ((self: window, x: number, y: number) -> ()),
        --- size getter
    size: ((self: window) -> (number, number)
        --- size setter
        & (self: window, w: number, h: number) -> ()),
}
type window_struct = {
    create: (title: string, x: number, y: number, w: number, h: number, window_flags: number) -> window,
    FLAGS: window_flags,
    UNDEFINED: number,
    CENTERED: number,
}
export type event_type = {
    QUIT: number,
    KEYUP: number,
    KEYDOWN: number,
    DROPFILE: number,
    DROPTEXT: number,
    FINGERUP: number,
    LASTEVENT: number,
    USEREVENT: number,
    DROPBEGIN: number,
    TEXTINPUT: number,
    FIRSTEVENT: number,
    SYSWMEVENT: number,
    FINGERDOWN: number,
    MOUSEWHEEL: number,
    WINDOWEVENT: number,
    JOYBUTTONUP: number,
    MOUSEMOTION: number,
    TEXTEDITING: number,
    DISPLAYEVENT: number,
    MOUSEBUTTONUP: number,
    MOUSEBUTTONDOWN: number,
}
export type window_flags = {
    SHOWN: number,
    RESIZABLE: number,
    METAL: number,
    HIDDEN: number,
    OPENGL: number,
    VULKAN: number,
    FOREIGN: number,
    TOOLTIP: number,
    UTILITY: number,
    MAXIMIZED: number,
    MINIMIZED: number,
    BORDERLESS: number,
    FULLSCREEN: number,
    MENU: number,
    INPUT_FOCUS: number,
    MOUSE_FOCUS: number,
    SKIP_TASKBAR: number,
    MOUSE_CAPTURE: number,
    MOUSE_GRABBED: number,
    INPUT_GRABBED: number,
    ALLOW_HIGHDPI: number,
    ALWAYS_ON_TOP: number,
    KEYBOARD_GRABBED: number,
    FULLSCREEN_DESKTOP: number,
}
export type RendererFlags = {
    SOFTWARE: number,
    ACCELERATED: number,
    PRESENTVSYNC: number,
    TARGETTEXTURE: number,
}
export type color = {r: number, g: number, b: number, a: number}
export type desktop = {
    Init: () -> (),
    GetError: () -> string,
    Quit: () -> (),
    window: window_struct,
    event: event_struct,
    rect: rect_struct,
    EVENT_TYPE: event_type,
    RENDERER_FLAGS: RendererFlags,
    poll_event: (event: event) -> boolean,
    WINDOWPOS_CENTERED: number,
    WINDOWPOS_UNDEFINED: number,
}

return dllimport("minluau.modules.dll", "import_desktop")() :: desktop
