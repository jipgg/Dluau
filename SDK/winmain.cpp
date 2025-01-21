#include <Windows.h>
#include <iostream>
#include <string_view>
#include <ranges>
#include <lumin.h>
#include <vector>
#include <thread>
#include <chrono>
#include <span>
using namespace std::chrono_literals;
extern int lumin_main(std::span<std::string_view> args);

static void clear_input_buffer() {
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    if (hin != INVALID_HANDLE_VALUE) {
        FlushConsoleInputBuffer(hin);
    }
}
static bool simulate_key_press(int key) {
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    if (hin == INVALID_HANDLE_VALUE) return false;
    INPUT_RECORD ir[2] = {};
    //down
    ir[0].EventType = KEY_EVENT;
    ir[0].Event.KeyEvent.bKeyDown = TRUE;
    ir[0].Event.KeyEvent.wVirtualKeyCode = key;
    ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
    ir[0].Event.KeyEvent.uChar.AsciiChar = '\r';
    ir[0].Event.KeyEvent.dwControlKeyState = 0;
    //up
    ir[1].EventType = KEY_EVENT;
    ir[1].Event.KeyEvent.bKeyDown = FALSE;
    ir[1].Event.KeyEvent.wVirtualKeyCode = key;
    ir[1].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
    ir[1].Event.KeyEvent.uChar.AsciiChar = '\r';
    ir[1].Event.KeyEvent.dwControlKeyState = 0;
    DWORD written;
    WriteConsoleInput(hin, ir, 2, &written);
    return true;
}
static bool redirect_console_output(bool try_attach_console = false) {
    bool alloc = false;
    if (try_attach_console) {
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            AllocConsole();
            alloc = true;
        } else {
        }
    } else {
        AllocConsole();
        alloc = true;
    }
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio(true);
    HANDLE ready_event = OpenEvent(EVENT_MODIFY_STATE, FALSE, "ChildProcessReady");
    if (ready_event) {
        SetEvent(ready_event);
        CloseHandle(ready_event);
    }
    return alloc;
}
void enable_virtual_terminal_processing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const bool alloc = redirect_console_output();
    const bool pause_after_execution = true;
    auto range = std::views::split(std::string_view(lpCmdLine), ' ');
    std::vector<std::string_view> args;
    for (auto sub : range) args.emplace_back(sub.begin(), sub.end());
    const int exit_code = lumin_main(args);
    std::cout << std::format("\nProgram exited with code {}.\n", exit_code);
    clear_input_buffer();
    std::cout << std::flush;
    std::cerr << std::flush;
    if (pause_after_execution) {
        if (alloc) std::system("pause");
    } else {
        if (not alloc) simulate_key_press(VK_RETURN);
    }
    FreeConsole();
    return 0;
}
