#include <iostream>
// Must be here due to collision with boost
#include "oauth.h"
#include "windows.h"
#include <cpprest/base_uri.h>
#include "Client.h"

HHOOK _hook;
KBDLLHOOKSTRUCT kdbStruct;

Client *client;
int volume;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Ignore
    if (nCode < 0) {
        return CallNextHookEx(_hook, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYDOWN) {
        kdbStruct = *((KBDLLHOOKSTRUCT*)lParam);
        if (kdbStruct.vkCode == VK_VOLUME_UP) {
            // Blocks the keypress
			client->set_volume(volume++);

            return 1;
        }
        else if(kdbStruct.vkCode == VK_VOLUME_DOWN) {
            // Blocks the keypress
			client->set_volume(volume--);
            return 1;
        }

    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}


int main() {
	std::wcout << "Starting..." << '\n';
	Config config;
	if (!config.is_valid()) {
		std::wcout << "Failed to read config file." << std::endl;
		std::cin.get();
		return 1;
	}
	web::json::value token = get_token(config);
	if (token.is_null()) {
		std::wcout << "Failed to connect to spotify, exiting..." << std::endl;
		std::cin.get();
		return 1;
	}
	Client temp(token, config);
	client = &temp;
	volume = client->get_current_playing_volume();
	std::wcout << "Connected to spotify successfully!" << std::endl;
    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    // Continuously looks for new keyboard inputs
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {};

    UnhookWindowsHookEx(_hook);
    return 0;
}