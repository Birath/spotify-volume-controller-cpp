#include <iostream>
#include "spotify.h"
#include "windows.h"
#include <cpprest/base_uri.h>

HHOOK _hook;
KBDLLHOOKSTRUCT kdbStruct;

void start_up();

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
			start_up();
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

void start_up() {
	web::uri authorize_url(L"https://accounts.spotify.com/authorize");
	request(authorize_url);
	
}


int main() {
	get_authorization_code(Config::REDIRECT_URI);
	

	std::cin.get();
	return 0;
    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    // Continuously looks for new keyboard inputs
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {};

    UnhookWindowsHookEx(_hook);
    return 0;
}