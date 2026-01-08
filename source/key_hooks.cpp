#include <bit>
#include <memory>
#include <utility>

#include "key_hooks.h"

#include <fmt/core.h>

#ifdef __linux__
#  include <X11/Xlib.h>
#  include <uiohook.h>
#endif

#ifdef _WIN32
#  include <libloaderapi.h>
#  include <minwindef.h>
#  include <windef.h>
#  include <winuser.h>
#endif
#include "VolumeController.h"
namespace spotify_volume_controller::key_hooks
{
namespace
{
#ifdef _WIN32
std::unique_ptr<VolumeController> g_controller {};  // NOLINT
HHOOK hook;  // NOLINT

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
#else
void volume_callback(uiohook_event* const event, void* user_data)
{
  auto* controller = static_cast<VolumeController*>(user_data);
  switch (event->type) {
    case EVENT_KEY_PRESSED: {
      if (event->data.keyboard.keycode == controller->volume_up_keycode()) {
        controller->increase_volume();
      } else if (event->data.keyboard.keycode == controller->volume_down_keycode()) {
        controller->decrease_volume();
      }
      break;
    }
    default: {
      break;
    }
  }
}

void print_callback(uiohook_event* const event, void* user_data)
{
  (void)user_data;
  switch (event->type) {
    case EVENT_KEY_PRESSED: {
      fmt::println("Key pressed {}, rawcode={}", event->data.keyboard.keycode, event->data.keyboard.rawcode);
      break;
    }
    default: {
      break;
    }
  }
}

#endif
}  // namespace
#ifdef _WIN32
void start_volume_hook(std::unique_ptr<VolumeController> controller)
{
  g_controller = std::move(controller);
  hook = SetWindowsHookExA(WH_KEYBOARD_LL, volume_callback, GetModuleHandle(nullptr), 0);
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {};
  UnhookWindowsHookEx(hook);
}

void start_print_vkey()
{
  hook = SetWindowsHookExA(WH_KEYBOARD_LL, print_v_key, GetModuleHandle(nullptr), 0);
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {};

  UnhookWindowsHookEx(hook);
}
#else

void print_hook_run_status(int status)
{
  switch (status) {
    case UIOHOOK_SUCCESS:
      // Everything is ok.
      break;

    // System level errors.
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
      fmt::println("Failed to allocate memory. ({})", status);
      break;

    // X11 specific errors.
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
      fmt::println("Failed to open X11 display. ({})", status);
      break;

    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
      fmt::println("Unable to locate XRecord extension. ({})", status);
      break;

    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
      fmt::println("Unable to allocate XRecord range. ({})", status);
      break;

    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
      fmt::println("Unable to allocate XRecord context. ({})", status);
      break;

    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
      fmt::println("Failed to enable XRecord context. ({})", status);
      break;
    default:
      fmt::println("Unhandled uiohook error. Are you on an unsupported platform? ({})", status);
      break;
  }
}

void start_volume_hook(std::unique_ptr<VolumeController> controller)
{
  hook_set_dispatch_proc(&volume_callback, controller.get());
  print_hook_run_status(hook_run());
}
void start_print_vkey()
{
  hook_set_dispatch_proc(&print_callback, nullptr);
  print_hook_run_status(hook_run());
}
#endif

}  // namespace spotify_volume_controller::key_hooks
