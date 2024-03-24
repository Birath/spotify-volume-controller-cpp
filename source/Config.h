#pragma once
#include <filesystem>

#include <cpprest/json.h>
#include <cpprest/uri.h>

#include "data_types.h"

namespace spotify_volume_controller
{

class Config
{
public:
  Config();
  Config(const std::string& path);

  ~Config() = default;

  utility::string_t get_client_id() const;
  utility::string_t get_client_secret() const;
  utility::string_t get_redirect_url() const;
  keycode get_volume_up() const;
  keycode get_volume_down() const;
  volume volume_increment() const;

  bool is_default_down() const;
  bool is_default_up() const;

  bool should_print_keys() const;
  bool is_valid();
  bool hide_window() const;

  [[nodiscard]] std::filesystem::path config_directory() const;

  static const utility::string_t SCOPES;
  static const web::uri BASE_AUTHENTICATION_API_URI;

private:
  const utility::string_t DEFAULT_CALLBACK_URL = L"http://localhost:5000/callback";
  web::json::value config;
  std::filesystem::path directory;

  void get_user_input(utility::string_t prompt, utility::string_t& input, bool not_empty = false) const;
  void parse_config_file(std::string const& path);
};

}  // namespace spotify_volume_controller