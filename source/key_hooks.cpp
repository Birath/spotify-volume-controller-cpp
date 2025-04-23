#include <bit>
#include <memory>
#include <utility>

#include "key_hooks.h"

#include <fmt/core.h>

#ifdef __linux__
#  include <X11/Xlib.h>
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
#ifdef _WIN32
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
#endif

Display* create_display()
{
  Display* display = XOpenDisplay(nullptr);
  int screen = XDefaultScreen(display);
  Window window = XCreateSimpleWindow(display,
                                      RootWindow(display, screen),
                                      10,
                                      10,
                                      200,
                                      200,
                                      1,
                                      BlackPixel(display, screen),
                                      WhitePixel(display, screen));
  XSelectInput(display, window, KeyPressMask);
  XMapWindow(display, window);
  return display;
}

void start_volume_hook(std::unique_ptr<VolumeController> controller)
{
  Display* display = create_display();
  while (true) {
    XEvent event;
    XNextEvent(display, &event);

    /* keyboard events */
    if (event.type == KeyPress) {
      if (event.xkey.keycode == controller->volume_down_keycode()) {
        controller->decrease_volume();
      } else if (event.xkey.keycode == controller->volume_up_keycode()) {
        controller->increase_volume();
      }
    }
  }
}
void start_print_vkey()
{
  Display* display = create_display();
  while (true) {
    XEvent event;
    XNextEvent(display, &event);

    /* keyboard events */
    if (event.type == KeyPress) {
      fmt::println("KeyPress: {}", event.xkey.keycode);
      /* exit on ESC key press */
      if (event.xkey.keycode == 0x09)
        break;
    }
  }
}
}  // namespace spotify_volume_controller::key_hooks