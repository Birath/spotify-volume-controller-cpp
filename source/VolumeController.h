#pragma once

#include "Client.h"
#include "Config.h"
namespace spotify_volume_controller
{

class VolumeController
{
public:
  VolumeController(const Config& config, Client& client);
  ~VolumeController();

  void set_desktop_device();

  [[nodiscard]] volume get_volume() const;
  [[nodiscard]] keycode volume_up_keycode() const;
  [[nodiscard]] keycode volume_down_keycode() const;
  void decrease_volume();
  void increase_volume();

  void start();
  void print_keys();

private:
  void set_volume(const volume to_volume);
  volume m_volume = 0;
  const Config m_config;
  Client m_client;
  const keycode m_volume_up_keycode;
  const keycode m_volume_down_keycode;
  std::optional<std::string> m_desktop_device_id = {};
};

}  // namespace spotify_volume_controller
