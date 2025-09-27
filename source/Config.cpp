#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Config.h"

#include <fmt/core.h>

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
constexpr std::string_view fetch_cooldown_key = "fetch_cooldown_ms";

constexpr std::chrono::milliseconds default_batch_delay {100};
constexpr uint32_t default_volume_increment = 1;
constexpr std::chrono::milliseconds default_fetch_cooldown {2000};
constexpr std::chrono::milliseconds min_fetch_cooldown {100};
constexpr std::string_view default_callback_url = "http://127.0.0.1:5000/callback";

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
  m_directory = std::filesystem::absolute(path).parent_path();
  if (config_file) {
    try {
      m_config = json::parse(config_file);
    } catch (const json::exception& e) {
      fmt::println(stderr, "Invalid config file: {}", e.what());
      m_config = json {};
    }
    return;
  }
  fmt::println("No config file found, creating.");
  std::ofstream new_config_file(m_directory / "config.json");
  std::string input {};

  get_user_input("Please enter your spotify client id", input, /*not_empty=*/true);
  m_config[client_id_key] = input;

  get_user_input("Please enter your spotify client secret", input, /*not_empty=*/true);
  m_config[client_secret_key] = input;

  get_user_input(fmt::format("Enter callback url, or leave empty for default ({}).", default_callback_url), input);
  if (input.empty()) {
    m_config[redirect_url_key] = default_callback_url;
  } else {
    m_config[redirect_url_key] = input;
  }

  get_user_input("Enter volume up virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    m_config[volume_up_key] = "default";
  } else {
    m_config[volume_up_key] = input;
  }
  get_user_input("Enter volume down virtual keycode as a number, or leave empty for default", input);
  if (input.empty()) {
    m_config[volume_down_key] = "default";
  } else {
    m_config[volume_down_key] = input;
  }
  m_config[print_keys_key] = false;
  m_config[hide_window_key] = false;

  m_config[volume_increment_key] = default_volume_increment;
  m_config[batch_delay_key] = default_batch_delay.count();
  m_config[fetch_cooldown_key] = default_fetch_cooldown.count();

  new_config_file << m_config;
  new_config_file.close();
}

std::string Config::get_client_id() const
{
  return m_config.at(client_id_key).template get<std::string>();
}
std::string Config::get_client_secret() const
{
  return m_config.at(client_secret_key).template get<std::string>();
}
std::string Config::get_redirect_url() const
{
  return m_config.at(redirect_url_key).template get<std::string>();
}

bool Config::should_print_keys() const
{
  return m_config.at(print_keys_key).template get<bool>();
}

keycode Config::get_volume_up() const
{
  if (!m_config.contains(volume_up_key)) {
    throw std::runtime_error(fmt::format("Missing {} config", volume_up_key));
  }
  json const v_up = m_config.at(volume_down_key);
  if (!v_up.is_number_integer()) {
    throw std::runtime_error(fmt::format("{} config is not a valid keycode", volume_up_key));
  }
  return v_up.template get<keycode>();
}

keycode Config::get_volume_down() const
{
  if (!m_config.contains(volume_down_key)) {
    throw std::runtime_error(fmt::format("Missing {} config", volume_down_key));
  }
  json const v_down = m_config.at(volume_down_key);

  if (!v_down.is_number_integer()) {
    throw std::runtime_error(fmt::format("{} config is not a valid keycode", volume_down_key));
  }
  return v_down.template get<keycode>();
}

bool Config::is_default_down() const
{
  if (!m_config.contains(volume_down_key)) {
    return false;
  }
  json const v_down = m_config.at(volume_down_key);
  return v_down.is_string() && v_down.template get<std::string>() == "default";
}

bool Config::is_default_up() const
{
  if (!m_config.contains(volume_down_key)) {
    return false;
  }
  json const v_up = m_config.at(volume_down_key);
  return v_up.is_string() && v_up.template get<std::string>() == "default";
}

bool Config::is_valid() const
{
  return !m_config.is_null();
}

bool Config::hide_window() const
{
  if (!m_config.contains(hide_window_key)) {
    return false;
  }

  return m_config.at(hide_window_key).template get<bool>();
}

[[nodiscard]] std::filesystem::path Config::config_directory() const
{
  return m_directory;
}

volume Config::volume_increment() const
{
  if (!m_config.contains(volume_increment_key.data())) {
    return volume(default_volume_increment);
  }

  return volume(m_config.at(volume_increment_key.data()).template get<volume_t>());
}

std::chrono::milliseconds Config::batch_delay() const
{
  return std::chrono::milliseconds(m_config.value(batch_delay_key, default_batch_delay.count()));
}

std::chrono::milliseconds Config::fetch_cooldown() const
{
  return std::max(std::chrono::milliseconds(m_config.value(fetch_cooldown_key, default_fetch_cooldown.count())),
                  min_fetch_cooldown);
}

void Config::get_user_input(const std::string_view prompt, std::string& input, bool not_empty)
{
  input.clear();
  fmt::println("{}", prompt);
  std::getline(std::cin, input);
  while (not_empty && input.empty()) {
    fmt::println("Input can't be empty");
    std::getline(std::cin, input);
  }
}

}  // namespace spotify_volume_controller