#include <fstream>
#include <iostream>
#include <string>

#include "Config.h"

#include <fmt/format.h>

constexpr std::string_view VOLUME_INCREMENT_KEY = "volume_increment";

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

void Config::parse_config_file(const std::filesystem::path& path)
{
  std::ifstream config_file(path);
  directory = std::filesystem::absolute(path).parent_path();
  if (config_file) {
    try {
      config = json::parse(config_file);
    } catch (const json::exception& e) {
      std::cout << "Invalid config file." << std::endl;
      std::cerr << e.what() << std::endl;
      config = json {};
    }
    return;
  }
  std::cout << "No config file found, creating." << std::endl;
  std::ofstream new_config_file(directory / "config.json");
  std::string input {};

  get_user_input("Please enter your spotify client id", input, true);
  config["client_id"] = input;

  get_user_input("Please enter your spotify client secret", input, true);
  config["client_secret"] = input;

  get_user_input(fmt::format("Enter callback url, or leave empty for default ({}).", Config::DEFAULT_CALLBACK_URL),
                 input);
  if (input.empty()) {
    config["redirect_url"] = Config::DEFAULT_CALLBACK_URL;
  } else {
    config["redirect_url"] = input;
  }

  get_user_input("Enter volume up virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config["volume_up"] = "default";
  } else {
    config["volume_up"] = input;
  }
  get_user_input("Enter volume down virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config["volume_down"] = "default";
  } else {
    config["volume_down"] = input;
  }
  config["print_keys"] = false;
  config["hide_window"] = false;

  config[VOLUME_INCREMENT_KEY.data()] = 1;

  new_config_file << config;
  new_config_file.close();
}

std::string Config::get_client_id() const
{
  return config.at("client_id").template get<std::string>();
}
std::string Config::get_client_secret() const
{
  return config.at("client_secret").template get<std::string>();
}
std::string Config::get_redirect_url() const
{
  return config.at("redirect_url").template get<std::string>();
}

bool Config::should_print_keys() const
{
  return config.at("print_keys").template get<bool>();
}

keycode Config::get_volume_up() const
{
  if (!config.contains("volume_up")) {
    throw std::runtime_error("Missing volume_up config");
  }
  json v_up = config.at("volume_up");
  if (!v_up.is_number_integer()) {
    throw std::runtime_error("volume_up config is not a valid keycode");
  }
  return v_up.template get<keycode>();
}

keycode Config::get_volume_down() const
{
  if (!config.contains("volume_down")) {
    throw std::runtime_error("Missing volume_down config");
  }
  json v_down = config.at("volume_down");

  if (!v_down.is_number_integer()) {
    throw std::runtime_error("volume_down config is not a valid keycode");
  }
  return v_down.template get<keycode>();
}

bool Config::is_default_down() const
{
  if (!config.contains("volume_down")) {
    return false;
  }
  json v_down = config.at("volume_down");
  return v_down.is_string() && v_down.template get<std::string>() == "default";
}

bool Config::is_default_up() const
{
  if (!config.contains("volume_up")) {
    return false;
  }
  json v_up = config.at("volume_up");
  return v_up.is_string() && v_up.template get<std::string>() == "default";
}

bool Config::is_valid() const
{
  return !config.is_null();
}

bool Config::hide_window() const
{
  if (!config.contains("hide_window")) {
    return false;
  }

  return config.at("hide_window").template get<bool>();
}

[[nodiscard]] std::filesystem::path Config::config_directory() const
{
  return directory;
}

volume Config::volume_increment() const
{
  if (!config.contains(VOLUME_INCREMENT_KEY.data())) {
    return 1;
  }

  return config.at(VOLUME_INCREMENT_KEY.data()).template get<volume_t>();
}

void Config::get_user_input(const std::string_view prompt, std::string& input, bool not_empty) const
{
  input.clear();
  std::cout << prompt << std::endl;
  std::getline(std::cin, input);
  while (not_empty && input.empty()) {
    std::cout << "Input can't be empty" << std::endl;
    std::getline(std::cin, input);
  }
}

}  // namespace spotify_volume_controller