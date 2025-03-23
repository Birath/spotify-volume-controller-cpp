#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Config.h"

#include <fmt/format.h>

#include "data_types.h"

namespace spotify_volume_controller
{

constexpr std::string_view client_id_key = "client_id";
constexpr std::string_view client_secret_key = "client_secret";
constexpr std::string_view hide_window_key = "hide_window";
constexpr std::string_view redirect_url_key = "redirect_url";
constexpr std::string_view print_keys_key = "print_keys";
constexpr std::string_view volume_increment_key = "volume_increment";
constexpr std::string_view volume_up_key = "volume_up";
constexpr std::string_view volume_down_key = "volume_down";
constexpr std::string_view batch_delay_key = "batch_delay_ms";

constexpr std::chrono::milliseconds default_batch_delay {100};
constexpr uint32_t default_volume_increment = 1;

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
  config[client_id_key] = input;

  get_user_input("Please enter your spotify client secret", input, true);
  config[client_secret_key] = input;

  get_user_input(fmt::format("Enter callback url, or leave empty for default ({}).", Config::DEFAULT_CALLBACK_URL),
                 input);
  if (input.empty()) {
    config[redirect_url_key] = Config::DEFAULT_CALLBACK_URL;
  } else {
    config[redirect_url_key] = input;
  }

  get_user_input("Enter volume up virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config[volume_up_key] = "default";
  } else {
    config[volume_up_key] = input;
  }
  get_user_input("Enter volume down virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    config[volume_down_key] = "default";
  } else {
    config[volume_down_key] = input;
  }
  config[print_keys_key] = false;
  config[hide_window_key] = false;

  config[volume_increment_key] = default_volume_increment;
  config[batch_delay_key] = default_batch_delay.count();

  new_config_file << config;
  new_config_file.close();
}

std::string Config::get_client_id() const
{
  return config.at(client_id_key).template get<std::string>();
}
std::string Config::get_client_secret() const
{
  return config.at(client_secret_key).template get<std::string>();
}
std::string Config::get_redirect_url() const
{
  return config.at(redirect_url_key).template get<std::string>();
}

bool Config::should_print_keys() const
{
  return config.at(print_keys_key).template get<bool>();
}

keycode Config::get_volume_up() const
{
  if (!config.contains(volume_up_key)) {
    throw std::runtime_error(std::format("Missing {} config", volume_up_key));
  }
  json v_up = config.at(volume_down_key);
  if (!v_up.is_number_integer()) {
    throw std::runtime_error(fmt::format("{} config is not a valid keycode", volume_up_key));
  }
  return v_up.template get<keycode>();
}

keycode Config::get_volume_down() const
{
  if (!config.contains(volume_down_key)) {
    throw std::runtime_error(fmt::format("Missing {} config", volume_down_key));
  }
  json v_down = config.at(volume_down_key);

  if (!v_down.is_number_integer()) {
    throw std::runtime_error(std::format("{} config is not a valid keycode", volume_down_key));
  }
  return v_down.template get<keycode>();
}

bool Config::is_default_down() const
{
  if (!config.contains(volume_down_key)) {
    return false;
  }
  json v_down = config.at(volume_down_key);
  return v_down.is_string() && v_down.template get<std::string>() == "default";
}

bool Config::is_default_up() const
{
  if (!config.contains(volume_down_key)) {
    return false;
  }
  json v_up = config.at(volume_down_key);
  return v_up.is_string() && v_up.template get<std::string>() == "default";
}

bool Config::is_valid() const
{
  return !config.is_null();
}

bool Config::hide_window() const
{
  if (!config.contains(hide_window_key)) {
    return false;
  }

  return config.at(hide_window_key).template get<bool>();
}

[[nodiscard]] std::filesystem::path Config::config_directory() const
{
  return directory;
}

volume Config::volume_increment() const
{
  if (!config.contains(volume_increment_key.data())) {
    return default_volume_increment;
  }

  return config.at(volume_increment_key.data()).template get<volume_t>();
}

std::chrono::milliseconds Config::batch_delay() const
{
  return std::chrono::milliseconds(config.value(batch_delay_key, default_batch_delay.count()));
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