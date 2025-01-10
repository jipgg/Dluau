#pragma once
namespace winutil {
bool simulate_key_press(int key);
bool redirect_console_output();
void enable_virtual_terminal_processing();
void configure_console_input();
}
