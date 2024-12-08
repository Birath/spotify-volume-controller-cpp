#include <string>

#include "VolumeController.h"

#include "key_hooks.h"
namespace spotify_volume_controller
{

VolumeController::VolumeController(const Config& config, Client& client)
    : m_config(config)
    , m_client(client)
    , m_volume_up_keycode(config.is_default_up() ? VK_VOLUME_UP : config.get_volume_up())
    , m_volume_down_keycode(config.is_default_down() ? VK_VOLUME_DOWN : config.get_volume_down())
{
}

VolumeController::~VolumeController() {}

void VolumeController::set_desktop_device()
{
  web::json::value desktop = m_client.get_desktop_player();
  if (desktop == web::json::value::Null)
    return;
  if (desktop.has_string_field(L"id")) {
    m_desktop_device_id = desktop.at(L"id").as_string();
  }
}

[[nodiscard]] volume VolumeController::get_volume() const
{
  return m_volume;
}

void VolumeController::decrease_volume()
{
  set_volume(m_volume - m_config.volume_increment());
}

void VolumeController::increase_volume()
{
  set_volume(m_volume + m_config.volume_increment());
}

void VolumeController::set_volume(const volume to_volume)
{
  volume new_volume = to_volume;
  if (!m_desktop_device_id.has_value()) {
    set_desktop_device();
    if (m_desktop_device_id.has_value()) {
      new_volume = m_client.get_device_volume(m_desktop_device_id.value()).value_or(volume(0));
    }
  }

  web::http::http_response result = m_client.set_volume(new_volume);
  if (result.status_code() == web::http::status_codes::NoContent) {
    m_volume = new_volume;
    return;
  } else if ((result.status_code() == web::http::status_codes::NotFound
              || result.status_code() == web::http::status_codes::Forbidden)
             && m_desktop_device_id.has_value())
  {
    result = m_client.set_device_volume(new_volume, m_desktop_device_id.value());
  }
  if (result.status_code() == web::http::status_codes::NoContent) {
    m_volume = new_volume;
    return;
  }
  m_client.print_error_message(result);
}

void VolumeController::start()
{
  key_hooks::start_volume_hook(this);
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
}  // namespace spotify_volume_controller
