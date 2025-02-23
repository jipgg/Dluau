#include "os.hpp"
#include <unordered_map>
#include <expected>
#include <format>

enum class Virtual_key {
    Mouse_button_1= 0x01,
    Mouse_button_2 = 0x02,
    Cancel = 0x03,
    Mouse_button_3 = 0x04,
    Mouse_button_4 = 0x05,
    Mouse_button_5 = 0x06,
    Backspace = 0x08,
    Tab = 0x09,
    Clear = 0x0c,
    Enter = 0x0d,
    Shift = 0x10,
    Control = 0x11,
    Alt = 0x12,
    Pause = 0x13,
    Caps_lock = 0x14,
    Escape = 0x1b,
    Space = 0x20,
    Page_up = 0x21,
    Page_down = 0x22,
    End = 0x23,
    Home = 0x24,
    Left_arrow = 0x25,
    Up_arrow = 0x26,
    Right_arrow = 0x27,
    Down_arrow = 0x28,
    Select = 0x29,
    Print = 0x2a,
    Execute = 0x2b,
    Print_screen = 0x2c,
    Insert = 0x2d,
    Delete = 0x2e,
    Help = 0x2f,
    Zero = 0x30,
    One = 0x31,
    Two = 0x32,
    Three = 0x33,
    Four = 0x34,
    Five = 0x35,
    Six = 0x36,
    Seven = 0x37,
    Eight = 0x38,
    Nine = 0x39,
    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4a,
    K = 0x4b,
    L = 0x4c,
    M = 0x4d,
    N = 0x4e,
    O = 0x4f,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5a,
    Left_super = 0x5b,
    Right_super = 0x5c,
    Numpad_0 = 0x60,
    Numpad_1 = 0x61,
    Numpad_2 = 0x62,
    Numpad_3 = 0x63,
    Numpad_4 = 0x64,
    Numpad_5 = 0x65,
    Numpad_6 = 0x66,
    Numpad_7 = 0x67,
    Numpad_8 = 0x68,
    Numpad_9 = 0x69,
    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7a,
    F12 = 0x7b,
    Left_shift = 0xa0,
    Right_shift = 0xa1,
    Left_control = 0xa2,
    Right_control = 0xa3,
    Left_alt = 0xa4,
    Right_alt = 0xa5,

};
static const std::unordered_map<std::string_view, Virtual_key> key_name_map {
    {"mouse1", Virtual_key::Mouse_button_1},
    {"mouse2", Virtual_key::Mouse_button_2},
    {"mouse3", Virtual_key::Mouse_button_3},
    {"mouse4", Virtual_key::Mouse_button_4},
    {"mouse5", Virtual_key::Mouse_button_5},
    {"backspace", Virtual_key::Backspace},
    {"tab", Virtual_key::Tab},
    {"clear", Virtual_key::Clear},
    {"enter", Virtual_key::Enter},
    {"shift", Virtual_key::Shift},
    {"ctrl", Virtual_key::Control},
    {"alt", Virtual_key::Alt},
    {"pause", Virtual_key::Pause},
    {"caps lock", Virtual_key::Caps_lock},
    {"esc", Virtual_key::Escape},
    {"space", Virtual_key::Space},
    {"page up", Virtual_key::Page_up},
    {"page dwn", Virtual_key::Page_down},
    {"end", Virtual_key::End},
    {"home", Virtual_key::Home},
    {"left", Virtual_key::Left_arrow},
    {"up", Virtual_key::Up_arrow},
    {"right", Virtual_key::Right_arrow},
    {"down", Virtual_key::Down_arrow},
    {"select", Virtual_key::Select},
    {"print", Virtual_key::Print},
    {"execute", Virtual_key::Execute},
    {"print screen", Virtual_key::Print_screen},
    {"ins", Virtual_key::Insert},
    {"del", Virtual_key::Delete},
    {"help", Virtual_key::Help},
    {"0", Virtual_key::Zero},
    {"1", Virtual_key::One},
    {"2", Virtual_key::Two},
    {"3", Virtual_key::Three},
    {"4", Virtual_key::Four},
    {"5", Virtual_key::Five},
    {"6", Virtual_key::Six},
    {"7", Virtual_key::Seven},
    {"8", Virtual_key::Eight},
    {"9", Virtual_key::Nine},
    {"a", Virtual_key::A},
    {"b", Virtual_key::B},
    {"c", Virtual_key::C},
    {"d", Virtual_key::D},
    {"e", Virtual_key::E},
    {"f", Virtual_key::F},
    {"g", Virtual_key::G},
    {"h", Virtual_key::H},
    {"i", Virtual_key::I},
    {"j", Virtual_key::J},
    {"k", Virtual_key::K},
    {"l", Virtual_key::L},
    {"m", Virtual_key::M},
    {"n", Virtual_key::N},
    {"o", Virtual_key::O},
    {"p", Virtual_key::P},
    {"q", Virtual_key::Q},
    {"r", Virtual_key::R},
    {"s", Virtual_key::S},
    {"t", Virtual_key::T},
    {"u", Virtual_key::U},
    {"v", Virtual_key::V},
    {"w", Virtual_key::W},
    {"x", Virtual_key::X},
    {"y", Virtual_key::Y},
    {"z", Virtual_key::Z},
    {"lsuper", Virtual_key::Left_super},
    {"rsuper", Virtual_key::Right_super},
    {"num0", Virtual_key::Numpad_0},
    {"num1", Virtual_key::Numpad_0},
    {"num2", Virtual_key::Numpad_0},
    {"num3", Virtual_key::Numpad_0},
    {"num4", Virtual_key::Numpad_0},
    {"num5", Virtual_key::Numpad_0},
    {"num6", Virtual_key::Numpad_0},
    {"num7", Virtual_key::Numpad_0},
    {"num8", Virtual_key::Numpad_0},
    {"num9", Virtual_key::Numpad_0},
    {"f1", Virtual_key::F1},
    {"f2", Virtual_key::F2},
    {"f3", Virtual_key::F3},
    {"f4", Virtual_key::F4},
    {"f5", Virtual_key::F5},
    {"f6", Virtual_key::F6},
    {"f7", Virtual_key::F7},
    {"f8", Virtual_key::F8},
    {"f9", Virtual_key::F9},
    {"f10", Virtual_key::F10},
    {"f11", Virtual_key::F11},
    {"f12", Virtual_key::F12},
    {"lshift", Virtual_key::Left_shift},
    {"rshift", Virtual_key::Right_shift},
    {"lctrl", Virtual_key::Left_control},
    {"rctrl", Virtual_key::Right_control},
    {"lalt", Virtual_key::Left_alt},
    {"ralt", Virtual_key::Right_alt},
};

static auto to_virtual_key(std::string_view str) -> std::expected<Virtual_key, std::string> {
    if (not key_name_map.contains(str)) {
        return std::unexpected(std::format("unknown key '{}'", str));
    }
    return key_name_map.at(str);
}

#ifdef _WIN32
auto os::keydown(lua_State* L) -> int {
    constexpr int down_mask = 0x8000;
    constexpr int toggle_mask = 0x0001;
    auto key = to_virtual_key(luaL_checkstring(L, 1));
    if (!key) dluau::arg_error(L, 1, key.error());
    const bool down = (GetAsyncKeyState(static_cast<int>(*key)) & down_mask) != 0;
    lua_pushboolean(L, down);
    return 1;
}
static auto to_mb_option(std::string_view sv) -> long {
    if (sv == "ok") return MB_OK;
    else if (sv == "ok cancel") return MB_OKCANCEL;
    else if (sv == "yes no") return MB_YESNO;
    else if (sv == "abort retry ignore") return MB_ABORTRETRYIGNORE;
    else if (sv =="retry cancel") return MB_RETRYCANCEL;
    else if (sv == "cancel try continue") return MB_CANCELTRYCONTINUE;
    else if (sv == "yes no cancel") return MB_YESNOCANCEL;
    return MB_OK;
}
static auto id_to_string(int id) -> std::string_view {
    switch(id) {
        case IDABORT: return "abort";
        case IDCANCEL: return "cancel";
        case IDCONTINUE: return "continue";
        case IDIGNORE: return "ignore";
        case IDNO: return "no";
        case IDRETRY: return "retry";
        case IDTRYAGAIN: return "try";
        case IDYES: return "yes";
        default: return "ok";
    }
}
auto os::messagebox(lua_State *L) -> int {
    auto id = MessageBox(nullptr, luaL_optstring(L, 2, ""), luaL_checkstring(L, 1), to_mb_option(luaL_optstring(L, 3, "ok")));
    dluau::push(L, id_to_string(id));
    return 1;
}
#endif
