#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>

#include <nlohmann/json_fwd.hpp>

#include "data_types.h"

namespace spotify_volume_controller
{
// for convenience
using json = nlohmann::json;

class Config
{
public:
  Config();
  Config(const Config&) = default;
  Config(Config&&) = default;
  Config& operator=(const Config&) = default;
  Config& operator=(Config&&) = default;
  explicit Config(const std::string& path);

  ~Config() = default;

  std::string get_client_id() const;
  std::string get_client_secret() const;
  std::string get_redirect_url() const;
  keycode get_volume_up() const;
  keycode get_volume_down() const;
  volume volume_increment() const;
  std::chrono::milliseconds batch_delay() const;
  std::chrono::milliseconds poll_rate() const;

  bool is_default_down() const;
  bool is_default_up() const;

  bool should_print_keys() const;
  bool is_valid() const;
  bool hide_window() const;

  [[nodiscard]] std::filesystem::path config_directory() const;

private:
  static constexpr std::string_view default_callback_url = "http://localhost:5000/callback";
  json m_config;
  std::filesystem::path m_directory;

  static void get_user_input(std::string_view prompt, std::string& input, bool not_empty = false);
  void parse_config_file(const std::filesystem::path& path);
};

}  // namespace spotify_volume_controller