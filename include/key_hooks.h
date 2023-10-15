#pragma once
#include "windows.h"
#include "Client.h"
#include "VolumeController.h"

HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
VolumeController* m_controller = nullptr;

LRESULT CALLBACK VolumeCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(_hook, nCode, wParam, lParam);
	}

	if (wParam == WM_KEYDOWN) {
		kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
		if (kbdStruct.vkCode == m_controller->VOLUME_UP) {
			m_controller->increase_volume();
			return 1;
		}
		else if (kbdStruct.vkCode == m_controller->VOLUME_DOWN) {
			m_controller->decrease_volume();
			return 1;
		}
	}
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK PrintVKey(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(_hook, nCode, wParam, lParam);
	}
	if (wParam == WM_KEYDOWN) {
		kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
		std::wcout << kbdStruct.vkCode << std::endl;
	}
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void start_volume_hook(VolumeController *controller) {
	m_controller = controller;
	_hook = SetWindowsHookExA(WH_KEYBOARD_LL, VolumeCallback, GetModuleHandle(NULL), 0);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {};
	delete m_controller;
	UnhookWindowsHookEx(_hook);
}

void start_print_vkey() {
	_hook = SetWindowsHookExA(WH_KEYBOARD_LL, PrintVKey, GetModuleHandle(NULL), 0);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {};

	UnhookWindowsHookEx(_hook);
}

