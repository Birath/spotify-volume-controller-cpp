#include <bit>
#include <utility>

#include "key_hooks.h"

#include <fmt/core.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <windef.h>
#include <winuser.h>

#include "VolumeController.h"
namespace spotify_volume_controller::key_hooks
{
namespace
{
HHOOK hook;  // NOLINT
std::unique_ptr<VolumeController> g_controller {};  // NOLINT

LRESULT CALLBACK volume_callback(int n_code, WPARAM w_param, LPARAM l_param)
{
  if (n_code < 0) {
    return CallNextHookEx(nullptr, n_code, w_param, l_param);
  }

  if (w_param == WM_KEYDOWN) {
    auto* keyboard_struct = std::bit_cast<KBDLLHOOKSTRUCT*>(l_param);
    if (keyboard_struct->vkCode == g_controller->volume_up_keycode()) {
      g_controller->increase_volume();
      return 1;
    }
    if (keyboard_struct->vkCode == g_controller->volume_down_keycode()) {
      g_controller->decrease_volume();
      return 1;
    }
  }
  return CallNextHookEx(nullptr, n_code, w_param, l_param);
}

LRESULT CALLBACK print_v_key(int n_code, WPARAM w_param, LPARAM l_param)
{
  if (n_code < 0) {
    return CallNextHookEx(nullptr, n_code, w_param, l_param);
  }
  if (w_param == WM_KEYDOWN) {
    auto* keyboard_struct = std::bit_cast<KBDLLHOOKSTRUCT*>(l_param);
    fmt::println("{}", keyboard_struct->vkCode);
  }
  return CallNextHookEx(nullptr, n_code, w_param, l_param);
}
}  // namespace

void start_volume_hook(std::unique_ptr<VolumeController> controller)
{
  g_controller = std::move(controller);
  hook = SetWindowsHookExA(WH_KEYBOARD_LL, volume_callback, GetModuleHandle(nullptr), 0);
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
  };
  UnhookWindowsHookEx(hook);
}

void start_print_vkey()
{
  hook = SetWindowsHookExA(WH_KEYBOARD_LL, print_v_key, GetModuleHandle(nullptr), 0);
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
  };

  UnhookWindowsHookEx(hook);
}

}  // namespace spotify_volume_controller::key_hooks