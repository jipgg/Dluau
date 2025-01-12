#include <Windows.h>
#include <cstdio>
#include <ios>
#include <iostream>

namespace winutil {
void configure_console_input() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get standard input handle. Error: " << GetLastError() << std::endl;
        return;
    }

    // Set console input mode
    DWORD mode;
    if (!GetConsoleMode(hStdin, &mode)) {
        std::cerr << "Failed to get console mode. Error: " << GetLastError() << std::endl;
        return;
    }

    // Enable input processing (e.g., line input, echo input, etc.)
    mode |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
    if (!SetConsoleMode(hStdin, mode)) {
        std::cerr << "Failed to set console mode. Error: " << GetLastError() << std::endl;
    }

    // Flush the input buffer
    FlushConsoleInputBuffer(hStdin);
}
bool simulate_key_press(int key) {
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
bool redirect_console_output() {
    bool alloc = false;
    if (not AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
        alloc = true;
    } 
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio(true);
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
}
