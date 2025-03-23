#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "Client.h"
#include "Config.h"
namespace spotify_volume_controller
{

class timer
{
public:

  [[nodiscard]] bool is_running() const { return m_is_running; }
  void start(const std::function<void(void)>& callback, std::chrono::milliseconds delay)
  {
    if (m_is_running) {
      return;
    }
    m_is_running = true;
    m_timer_thread = std::jthread
    {
      [callback, delay, this]()
      {
        std::this_thread::sleep_for(delay);
        callback();
        m_is_running = false;
      }
    };
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
  VolumeController& operator=(const VolumeController&) = delete;
  VolumeController& operator=(VolumeController&&) = delete;
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
  void set_volume(volume to_volume);
  void set_volume_loop();
  void set_volume_to_desktop_device_volume();
  volume m_volume = 0;
  const Config m_config;
  Client m_client;
  const keycode m_volume_up_keycode;
  const keycode m_volume_down_keycode;
  std::optional<std::string> m_desktop_device_id = {};
  std::thread m_client_thread;
  std::jthread m_notify_timer_thread;
  std::mutex m_volume_mutex;
  std::condition_variable m_volume_cv;
  std::queue<volume> m_volume_queue;
  timer m_notify_timer{};
};

}  // namespace spotify_volume_controller
