#include <iostream>
#include "windows.h"

HHOOK _hook;
KBDLLHOOKSTRUCT kdbStruct;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Ignore
    if (nCode < 0) {
        return CallNextHookEx(_hook, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYDOWN) {
        kdbStruct = *((KBDLLHOOKSTRUCT*)lParam);
        if (kdbStruct.vkCode == VK_VOLUME_UP) {
            std::cout << "Volume up" << std::endl;
            // Blocks the keypress
            return 1;
        }
        else if(kdbStruct.vkCode == VK_VOLUME_DOWN) {
            std::cout << "Volume down" << std::endl;
            // Blocks the keypress
            return 1;
        }

    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

int main() {
    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    // Continuously looks for new keyboard inputs
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
    };

    UnhookWindowsHookEx(_hook);
    return 0;
}