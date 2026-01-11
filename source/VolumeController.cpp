#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "VolumeController.h"

#include <cpr/response.h>
#include <cpr/status_codes.h>
#include <fmt/core.h>
#include <nlohmann/json_fwd.hpp>
#ifdef _WIN32
#  include <winuser.h>
#endif
#include "Client.h"
#include "Config.h"
#include "data_types.h"
#include "key_hooks.h"

namespace spotify_volume_controller
{
// for convenience
using json = nlohmann::json;
#ifdef _WIN32
constexpr keycode default_up_keycode = VK_VOLUME_UP;
constexpr keycode default_down_keycode = VK_VOLUME_DOWN;
#else
constexpr keycode default_up_keycode = 57390;
constexpr keycode default_down_keycode = 57392;
#endif

VolumeController::VolumeController(const Config& config, Client& client)
    : m_volume()
    , m_config(config)
    , m_client(client)
    , m_volume_up_keycode(config.is_default_up() ? default_up_keycode : config.get_volume_up())
    , m_volume_down_keycode(config.is_default_down() ? default_down_keycode : config.get_volume_down())
    , m_client_thread(std::thread(&VolumeController::set_volume_loop, this))
    , m_update_current_volume_thread(std::jthread(&VolumeController::update_current_volume_loop, this))
{
}

VolumeController::~VolumeController() = default;

void VolumeController::set_desktop_device()
{
  std::optional<std::string> const desktop = m_client.get_desktop_player_id();
  if (!desktop.has_value()) {
    return;
  }
  m_desktop_device_id = desktop;
}

void VolumeController::decrease_volume()
{
  {
    std::lock_guard const lock(m_volume_mutex);
    // Assume that the API call will succeed
    m_volume_queue.push(volume_change::decrease);
  }
  m_update_current_volume_cv.notify_one();
  if (m_config.batch_delay() > std::chrono::milliseconds(0)) {
    m_notify_timer.start([this]() { m_volume_cv.notify_one(); }, m_config.batch_delay());
  } else {
    m_volume_cv.notify_one();
  }
}

void VolumeController::increase_volume()
{
  {
    std::lock_guard const lock(m_volume_mutex);
    // Assume that the API call will succeed
    m_volume_queue.push(volume_change::increase);
  }
  m_update_current_volume_cv.notify_one();
  if (m_config.batch_delay() > std::chrono::milliseconds(0)) {
    m_notify_timer.start([this]() { m_volume_cv.notify_one(); }, m_config.batch_delay());
  } else {
    m_volume_cv.notify_one();
  }
}

void VolumeController::set_volume_to_desktop_device_volume()
{
  if (!m_desktop_device_id.has_value()) {
    set_desktop_device();
  }
  if (m_desktop_device_id.has_value()) {
    m_volume = m_client.get_device_volume(m_desktop_device_id.value()).value_or(volume(0));
  }
}

void VolumeController::set_volume(const volume new_volume)
{
  cpr::Response result = m_client.set_volume(new_volume);
  if (result.status_code == cpr::status::HTTP_NO_CONTENT) {
    return;
  }
  if ((result.status_code == cpr::status::HTTP_NOT_FOUND || result.status_code == cpr::status::HTTP_FORBIDDEN
       || result.status_code == cpr::status::HTTP_LENGTH_REQUIRED)
      && m_desktop_device_id.has_value())
  {
    result = m_client.set_device_volume(new_volume, m_desktop_device_id.value());
  }
  if (result.status_code == cpr::status::HTTP_NO_CONTENT) {
    return;
  }
  Client::print_error_message(result);
}

void VolumeController::start()
{
  set_volume_to_desktop_device_volume();
  key_hooks::start_volume_hook(std::unique_ptr<VolumeController>(this));
}

void VolumeController::print_keys()
{
  key_hooks::start_print_vkey();
}

void VolumeController::set_volume_loop()
{
  while (true) {
    std::unique_lock lock(m_volume_mutex);
    m_volume_cv.wait(lock, [&] { return !m_volume_queue.empty(); });
    if (m_updating_current_volume) {
      std::unique_lock update_lock(m_update_current_volume_mutex);
      m_updating_current_volume_cv.wait(update_lock, [&] { return !m_updating_current_volume; });
    }
    volume new_volume = m_volume;
    while (!m_volume_queue.empty()) {
      const volume_change change = m_volume_queue.front();
      if (change == volume_change::increase) {
        new_volume += m_config.volume_increment();
      } else {
        new_volume -= m_config.volume_increment();
      }
      m_volume_queue.pop();
    }
    m_volume = new_volume;
    fmt::println("Setting volume to {}", new_volume.m_volume);
    set_volume(new_volume);
  }
}

void VolumeController::update_current_volume_loop()
{
  while (true) {
    {
      std::unique_lock update_lock(m_update_current_volume_mutex);
      m_update_current_volume_cv.wait(update_lock);
      m_updating_current_volume = true;
      fmt::println("Fetching current volume from Spotify API");
      std::optional<volume> current_volume = m_client.get_current_playing_volume();
      if (current_volume.has_value()) {
        m_volume = current_volume.value();
      }
      m_updating_current_volume_cv.notify_all();
      m_updating_current_volume = false;
    }
    std::this_thread::sleep_for(m_config.fetch_cooldown());
  }
}

}  // namespace spotify_volume_controller
