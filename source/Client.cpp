#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "Client.h"

#include <cpr/api.h>
#include <cpr/cprtypes.h>
#include <cpr/parameters.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <cpr/status_codes.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Config.h"
#include "data_types.h"
#include "oauth.h"

namespace spotify_volume_controller
{
// for convenience
using json = nlohmann::json;

constexpr std::string_view api_url {"https://api.spotify.com"};

Client::Client(token_t token_info, const Config& config)
    : m_token_info(std::move(token_info))
    , m_config(config)
{
}

[[nodiscard]] auto Client::api_request(const std::string_view endpoint) -> cpr::Response
{
  cpr::Url const url {fmt::format("{}{}", api_url, endpoint)};

  return cpr::Get(url, cpr::Bearer {get_token()});
}

[[nodiscard]] auto Client::put_api_request(const std::string_view endpoint, const cpr::Payload& payload)
    -> cpr::Response
{
  cpr::Url const url {fmt::format("{}{}", api_url, endpoint)};
  return cpr::Put(url, payload, cpr::Bearer {get_token()});
}

[[nodiscard]] auto Client::get_devices() -> std::optional<json>
{
  cpr::Response const response = api_request("/v1/me/player/devices");
  if (response.status_code != cpr::status::HTTP_OK) {
    print_error_message(response);
    return {};
  }
  json response_body = json::parse(response.text);
  return response_body.at("devices");
}

[[nodiscard]] auto Client::get_device_volume(const std::string_view device_id) -> std::optional<volume>
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value()) {
    return {};
  }

  for (auto&& device : devices.value()) {
    if (device.contains("id") && device_id == device.at("id").template get<std::string>()) {
      return device.contains("volume_percent") ? volume(device["volume_percent"].template get<volume_t>())
                                               : std::optional<volume> {};
    }
  }
  return {};
}

[[nodiscard]] auto Client::set_device_volume(volume volume, const std::string& device_id) -> cpr::Response
{
  cpr::Parameters payload {{"volume_percent", fmt::format("{}", volume.m_volume)}};
  if (!device_id.empty()) {
    payload.Add(cpr::Parameter {"device_id", device_id});
  }
  cpr::Url const url {fmt::format("{}/{}", api_url, "v1/me/player/volume")};
  return cpr::Put(url, payload, cpr::Bearer {get_token()}, cpr::Header {{"Content-Length", "0"}});
}

[[nodiscard]] auto Client::set_volume(volume volume) -> cpr::Response
{
  return set_device_volume(volume);
}

auto Client::get_current_playing_volume() -> std::optional<volume>
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value()) {
    return {};
  }

  for (auto&& device : devices.value()) {
    if (device.at("is_active").template get<bool>()) {
      // Volume percentage may be null according to documentation
      if (device["volume_percent"].is_number_integer()) {
        return volume(device["volume_percent"].template get<volume_t>());
      }
    }
  }
  return {};
}

[[nodiscard]] auto Client::get_desktop_player_id() -> std::optional<std::string>
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value()) {
    return {};
  }

  for (auto&& device : devices.value()) {
    if (device["type"].template get<std::string>() == "Computer") {
      return device.at("id").template get<std::string>();
    }
  }
  return {};
}

[[nodiscard]] auto Client::get_token() -> std::string
{
  if (oauth::token_is_expired(m_token_info)) {
    m_token_info = oauth::refresh_token(m_token_info, m_config);
  }

  return m_token_info.access_token;
}

void Client::print_error_message(const cpr::Response& response)
{
  std::cerr << "Error message: " << response.error.message << '\n';
  std::cerr << "Reason: " << response.reason << '\n';
}

}  // namespace spotify_volume_controller
