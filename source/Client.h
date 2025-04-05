#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <cpr/payload.h>
#include <cpr/response.h>
#include <nlohmann/json_fwd.hpp>

#include "Config.h"
#include "data_types.h"
namespace spotify_volume_controller
{
// for convenience
using json = nlohmann::json;

class Client
{
public:
  Client(token_t token_info, const Config& config);

  [[nodiscard]] auto api_request(std::string_view endpoint) -> cpr::Response;
  [[nodiscard]] auto put_api_request(std::string_view endpoint, const cpr::Payload& payload) -> cpr::Response;

  // Makes a request to the device endpoint
  [[nodiscard]] auto get_devices() -> std::optional<json>;

  [[nodiscard]] auto get_device_volume(std::string_view device_id) -> std::optional<volume>;

  // Sets the volume of device_id to *volume*
  [[nodiscard]] auto set_device_volume(volume volume, const std::string& device_id = "") -> cpr::Response;

  // Sets the volume of currently playing device to *volume*
  [[nodiscard]] auto set_volume(volume volume) -> cpr::Response;

  // Returns the volume percent of the current playing device
  auto get_current_playing_volume() -> std::optional<volume>;

  // Returns the ID of the first desktop device found
  [[nodiscard]] auto get_desktop_player_id() -> std::optional<std::string>;

  static void print_error_message(const cpr::Response& response);

private:
  [[nodiscard]] auto get_token() -> std::string;

  token_t m_token_info;
  Config m_config;
};

}  // namespace spotify_volume_controller
