#include <iostream>
#include <string>

#include "Config.h"

constexpr std::wstring_view VOLUME_INCREMENT_KEY = L"volume_increment";

using namespace web;
namespace spotify_volume_controller
{

Config::Config()
{
  parse_config_file("config.json");
}

Config::Config(const std::string& path)
{
  parse_config_file(path);
}

void Config::parse_config_file(const std::string& path)
{
  utility::ifstream_t config_file(path);
  directory = std::filesystem::absolute(path).parent_path();
  if (config_file) {
    try {
      config = web::json::value::parse(config_file);
    } catch (const web::json::json_exception& e) {
      std::cout << "Invalid config file." << std::endl;
      std::cerr << e.what() << std::endl;
      config = web::json::value::null();
    }
    return;
  }
  std::cout << "No config file found, creating." << std::endl;
  utility::ofstream_t new_config_file(directory / "config.json");
  utility::string_t input;

  get_user_input(L"Please enter your spotify client id", input, true);
  config[L"client_id"] = json::value::string(input);

  get_user_input(L"Please enter your spotify client secret", input, true);
  config[L"client_secret"] = json::value::string(input);

  get_user_input(L"Enter callback url, or leave empty for default (" + Config::DEFAULT_CALLBACK_URL + L").", input);
  if (input.empty()) {
    config[L"redirect_url"] = json::value::string(Config::DEFAULT_CALLBACK_URL);
  } else {
    config[L"redirect_url"] = json::value::string(input);
  }

  get_user_input(L"Enter volume up virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config[L"volume_up"] = json::value::string(L"default");
  } else {
    config[L"volume_up"] = json::value::string(input);
  }
  get_user_input(L"Enter volume down virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config[L"volume_down"] = json::value::string(L"default");
  } else {
    config[L"volume_down"] = json::value::string(input);
  }
  config[L"print_keys"] = json::value::boolean(false);
  config[L"hide_window"] = json::value::boolean(false);

  config[VOLUME_INCREMENT_KEY.data()] = json::value::number(1);

  new_config_file << config.serialize();
  new_config_file.close();
}

utility::string_t Config::get_client_id() const
{
  return config.at(L"client_id").as_string();
}
utility::string_t Config::get_client_secret() const
{
  return config.at(L"client_secret").as_string();
}
utility::string_t Config::get_redirect_url() const
{
  return config.at(L"redirect_url").as_string();
}

bool Config::should_print_keys() const
{
  return config.at(L"print_keys").as_bool();
}

keycode Config::get_volume_up() const
{
  if (!config.has_field(L"volume_up")) {
    throw std::runtime_error("Missing volume_up config");
  }
  web::json::value v_up = config.at(L"volume_up");
  if (!v_up.is_integer()) {
    throw std::runtime_error("volume_up config is not a valid keycode");
  }
  return v_up.as_number().to_uint32();
}

keycode Config::get_volume_down() const
{
  if (!config.has_field(L"volume_down")) {
    throw std::runtime_error("Missing volume_down config");
  }
  web::json::value v_down = config.at(L"volume_down");

  if (!v_down.is_integer()) {
    throw std::runtime_error("volume_down config is not a valid keycode");
  }
  return v_down.as_number().to_uint32();
}

bool Config::is_default_down() const
{
  if (!config.has_field(L"volume_down")) {
    return false;
  }
  web::json::value v_down = config.at(L"volume_down");
  return v_down.is_string() && v_down.as_string().compare(L"default") == 0;
}

bool Config::is_default_up() const
{
  if (!config.has_field(L"volume_up")) {
    return false;
  }
  web::json::value v_up = config.at(L"volume_up");
  return v_up.is_string() && v_up.as_string().compare(L"default") == 0;
}

bool Config::is_valid()
{
  return !config.is_null();
}

bool Config::hide_window() const
{
  if (!config.has_field(L"hide_window")) {
    return false;
  }

  return config.at(L"hide_window").as_bool();
}

[[nodiscard]] std::filesystem::path Config::config_directory() const
{
  return directory;
}

volume Config::volume_increment() const
{
  if (!config.has_field(VOLUME_INCREMENT_KEY.data())) {
    return 1;
  }

  return config.at(VOLUME_INCREMENT_KEY.data()).as_number().to_uint32();
}

void Config::get_user_input(utility::string_t prompt, utility::string_t& input, bool not_empty) const
{
  input.clear();
  std::wcout << prompt << std::endl;
  std::getline(std::wcin, input);
  while (not_empty && input.empty()) {
    std::cout << "Input can't be empty" << std::endl;
    std::getline(std::wcin, input);
  }
}

const utility::string_t Config::SCOPES = L"user-read-playback-state user-modify-playback-state";

const web::uri Config::BASE_AUTHENTICATION_API_URI(L"https://accounts.spotify.com/api/");

}  // namespace spotify_volume_controller