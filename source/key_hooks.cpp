#include <bit>
#include <memory>
#include <utility>

#include "key_hooks.h"

#include <fmt/core.h>
#include <uiohook.h>
#ifdef __linux__
#  include <X11/Xlib.h>
#endif

#include "VolumeController.h"
namespace spotify_volume_controller::key_hooks
{
namespace
{

void volume_callback(uiohook_event* const event, void* user_data)
{
  auto* controller = static_cast<VolumeController*>(user_data);
  switch (event->type) {
    case EVENT_KEY_PRESSED: {
      // Use rawcodes on Windows for backward compatibility
#ifdef _WIN32
      auto const keycode = event->data.keyboard.rawcode;
#else
      auto const keycode = event->data.keyboard.keycode;
#endif
      if (keycode == controller->volume_up_keycode()) {
        controller->increase_volume();
        event->mask |= MASK_CONSUMED;
      } else if (keycode == controller->volume_down_keycode()) {
        controller->decrease_volume();
        event->mask |= MASK_CONSUMED;
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

}  // namespace

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

}  // namespace spotify_volume_controller::key_hooks
