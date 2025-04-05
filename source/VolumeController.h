#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>

#include "Client.h"
#include "Config.h"
#include "data_types.h"
namespace spotify_volume_controller
{

class Timer
{
public:
  [[nodiscard]] auto is_running() const -> bool { return m_is_running; }
  void start(const std::function<void(void)>& callback, std::chrono::milliseconds delay)
  {
    if (m_is_running) {
      return;
    }
    m_is_running = true;
    m_timer_thread = std::jthread {[callback, delay, this]()
                                   {
                                     std::this_thread::sleep_for(delay);
                                     callback();
                                     m_is_running = false;
                                   }};
  }

private:
  std::jthread m_timer_thread;
  bool m_is_running = false;
};

class VolumeController
{
public:
  VolumeController(const VolumeController&) = delete;
  VolumeController(VolumeController&&) = delete;
  auto operator=(const VolumeController&) -> VolumeController& = delete;
  auto operator=(VolumeController&&) -> VolumeController& = delete;
  VolumeController(const Config& config, Client& client);
  ~VolumeController();

  void set_desktop_device();

  [[nodiscard]] auto get_volume() const -> volume;
  [[nodiscard]] auto volume_up_keycode() const -> keycode;
  [[nodiscard]] auto volume_down_keycode() const -> keycode;
  void decrease_volume();
  void increase_volume();

  void start();
  static void print_keys();

private:
  void set_volume(volume new_volume);
  void set_volume_loop();
  void set_volume_to_desktop_device_volume();
  volume m_volume;
  const Config m_config;
  Client m_client;
  const keycode m_volume_up_keycode;
  const keycode m_volume_down_keycode;
  std::optional<std::string> m_desktop_device_id;
  std::thread m_client_thread;
  std::jthread m_notify_timer_thread;
  std::mutex m_volume_mutex;
  std::condition_variable m_volume_cv;
  std::queue<volume> m_volume_queue;
  Timer m_notify_timer {};
};

}  // namespace spotify_volume_controller
