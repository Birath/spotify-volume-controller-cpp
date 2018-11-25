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
            std::cout << "Volume up" << std::endl;
            // Blocks the keypress
			client->set_volume(volume++);
			std::cout << "Was this blocked" << std::endl;

            return 1;
        }
        else if(kdbStruct.vkCode == VK_VOLUME_DOWN) {
            std::cout << "Volume down" << std::endl;
            // Blocks the keypress
			client->set_volume(volume--);

            return 1;
        }

    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void start_up() {
	web::uri authorize_url(L"https://accounts.spotify.com/authorize");
	//request(authorize_url);
	
}


int main() {
	Client bla(get_token());
	client = &bla;
	volume = client->get_current_playing_volume();

	std::cin.get();
	//return 0;
    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    // Continuously looks for new keyboard inputs
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {};

    UnhookWindowsHookEx(_hook);
    return 0;
}