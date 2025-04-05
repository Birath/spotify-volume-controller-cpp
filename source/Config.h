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
  auto operator=(const Config&) -> Config& = default;
  auto operator=(Config&&) -> Config& = default;
  explicit Config(const std::string& path);

  ~Config() = default;

  auto get_client_id() const -> std::string;
  auto get_client_secret() const -> std::string;
  auto get_redirect_url() const -> std::string;
  auto get_volume_up() const -> keycode;
  auto get_volume_down() const -> keycode;
  auto volume_increment() const -> volume;
  auto batch_delay() const -> std::chrono::milliseconds;

  auto is_default_down() const -> bool;
  auto is_default_up() const -> bool;

  auto should_print_keys() const -> bool;
  auto is_valid() const -> bool;
  auto hide_window() const -> bool;

  [[nodiscard]] auto config_directory() const -> std::filesystem::path;

private:
  static constexpr std::string_view default_callback_url = "http://localhost:5000/callback";
  json m_config;
  std::filesystem::path m_directory;

  static void get_user_input(std::string_view prompt, std::string& input, bool not_empty = false);
  void parse_config_file(const std::filesystem::path& path);
};

}  // namespace spotify_volume_controller