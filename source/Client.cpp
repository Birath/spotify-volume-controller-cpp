#include <codecvt>
#include <iostream>

#include "Client.h"

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "oauth.h"

// for convenience
using json = nlohmann::json;
namespace spotify_volume_controller
{

constexpr std::string_view API_URL {"https://api.spotify.com"};

Client::Client(token_t token_info, const Config& config)
    : m_token_info(token_info)
    , m_config(config)
{
}

[[nodiscard]] cpr::Response Client::api_request(const std::string_view endpoint)
{
  cpr::Url url {fmt::format("{}{}", API_URL, endpoint)};

  return cpr::Get(url, cpr::Bearer {get_token()});
}

[[nodiscard]] cpr::Response Client::put_api_request(const std::string_view endpoint, const cpr::Payload& payload)
{
  cpr::Url url {fmt::format("{}{}", API_URL, endpoint)};
  return cpr::Put(url, payload, cpr::Bearer {get_token()});
}

[[nodiscard]] std::optional<json> Client::get_devices()
{
  cpr::Response response = api_request("/v1/me/player/devices");
  if (response.status_code != cpr::status::HTTP_OK) {
    print_error_message(response);
    return {};
  }
  json response_body = json::parse(response.text);
  return response_body.at("devices");
}

[[nodiscard]] std::optional<volume> Client::get_device_volume(const std::string_view id)
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device.contains("id") && id == device.at("id").template get<std::string>()) {
      return device.contains("volume_percent") ? device["volume_percent"].template get<std::uint32_t>()
                                               : std::optional<volume> {};
    }
  }
  return {};
}

[[nodiscard]] cpr::Response Client::set_device_volume(volume volume, const std::string& device_id)
{
  cpr::Parameters payload {{"volume_percent", fmt::format("{}", volume.m_volume)}};
  if (!device_id.empty()) {
    payload.Add(cpr::Parameter {"device_id", device_id});
  }
  cpr::Url url {fmt::format("{}/{}", API_URL, "v1/me/player/volume")};
  return cpr::Put(url, payload, cpr::Bearer {get_token()}, cpr::Header {{"Content-Length", "0"}});
}

[[nodiscard]] cpr::Response Client::set_volume(volume volume)
{
  return set_device_volume(volume);
}

std::optional<volume> Client::get_current_playing_volume()
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device.at("is_active").template get<bool>()) {
      // Volume percentage may be null according to documentation
      if (device["volume_percent"].is_number_integer())
        return device["volume_percent"].template get<std::uint32_t>();
    }
  }
  return {};
}

[[nodiscard]] std::optional<std::string> Client::get_desktop_player_id()
{
  std::optional<json> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device["type"].template get<std::string>() == "Computer")
      return device.at("id").template get<std::string>();
  }
  return {};
}

[[nodiscard]] std::string Client::get_token()
{
  if (oauth::token_is_expired(m_token_info)) {
    m_token_info = oauth::refresh_token(m_token_info, m_config);
  }

  return m_token_info.access_token;
}

void Client::print_error_message(const cpr::Response& response) const
{
  // web::json::value body = get_json_response_body(response.text);
  // web::json::value error_body = body[L"error"];
  // std::cerr << "Status code: " << response.error.code << '\n';

  std::cerr << "Error message: " << response.error.message << '\n';
  // if (error_body.has_string_field(L"reason")) {
  std::cerr << "Reason: " << response.reason;
  // }
  std::cerr << std::endl;
}

}  // namespace spotify_volume_controller
