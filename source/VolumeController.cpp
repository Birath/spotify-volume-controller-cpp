#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "VolumeController.h"

#include <cpr/response.h>
#include <cpr/status_codes.h>
#include <nlohmann/json_fwd.hpp>
#include <winuser.h>

#include "Client.h"
#include "Config.h"
#include "data_types.h"
#include "key_hooks.h"

namespace spotify_volume_controller
{
// for convenience
using json = nlohmann::json;

VolumeController::VolumeController(const Config& config, Client& client)
    : m_volume()
    , m_config(config)
    , m_client(client)
    , m_volume_up_keycode(config.is_default_up() ? VK_VOLUME_UP : config.get_volume_up())
    , m_volume_down_keycode(config.is_default_down() ? VK_VOLUME_DOWN : config.get_volume_down())
    , m_client_thread(std::thread(&VolumeController::set_volume_loop, this))
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

[[nodiscard]] volume VolumeController::get_volume() const
{
  return m_volume;
}

void VolumeController::decrease_volume()
{
  {
    std::unique_lock const lock(m_volume_mutex);
    // Assume that the API call will succeed
    m_volume = m_volume - m_config.volume_increment();
    m_volume_queue.push(m_volume);
  }
  if (m_config.batch_delay() > std::chrono::milliseconds(0)) {
    m_notify_timer.start([this]() { m_volume_cv.notify_one(); }, m_config.batch_delay());
  } else {
    m_volume_cv.notify_one();
  }
}

void VolumeController::increase_volume()
{
  {
    std::unique_lock const lock(m_volume_mutex);
    // Assume that the API call will succeed
    m_volume = m_volume + m_config.volume_increment();
    m_volume_queue.push(m_volume);
  }
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

[[nodiscard]] keycode VolumeController::volume_up_keycode() const 
{
  return m_volume_up_keycode;
}

[[nodiscard]] keycode VolumeController::volume_down_keycode() const 
{
  return m_volume_down_keycode;
}

void VolumeController::set_volume_loop()
{
  while (true) {
    std::unique_lock lock(m_volume_mutex);
    m_volume_cv.wait(lock, [&] { return !m_volume_queue.empty(); });
    volume new_volume {0};
    while (!m_volume_queue.empty()) {
      new_volume = m_volume_queue.front();
      m_volume_queue.pop();
    }
    set_volume(new_volume);
  }
}

}  // namespace spotify_volume_controller
