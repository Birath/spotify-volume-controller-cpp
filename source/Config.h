#pragma once
#include <filesystem>

#include <nlohmann/json.hpp>

#include "data_types.h"

// for convenience
using json = nlohmann::json;
namespace spotify_volume_controller
{

class Config
{
public:
  Config();
  Config(const std::string& path);

  ~Config() = default;

  std::string get_client_id() const;
  std::string get_client_secret() const;
  std::string get_redirect_url() const;
  keycode get_volume_up() const;
  keycode get_volume_down() const;
  volume volume_increment() const;

  bool is_default_down() const;
  bool is_default_up() const;

  bool should_print_keys() const;
  bool is_valid() const;
  bool hide_window() const;

  [[nodiscard]] std::filesystem::path config_directory() const;

private:
  static constexpr std::string_view DEFAULT_CALLBACK_URL = "http://localhost:5000/callback";
  json config;
  std::filesystem::path directory;

  void get_user_input(const std::string_view prompt, std::string& input, bool not_empty = false) const;
  void parse_config_file(const std::filesystem::path& path);
};

}  // namespace spotify_volume_controller