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
  Client(token_t token_info, Config config);

  [[nodiscard]] cpr::Response api_request(std::string_view endpoint);
  [[nodiscard]] cpr::Response put_api_request(std::string_view endpoint, const cpr::Payload& payload);

  // Makes a request to the device endpoint
  [[nodiscard]] std::optional<json> get_devices();

  [[nodiscard]] std::optional<volume> get_device_volume(std::string_view device_id);

  // Sets the volume of device_id to *volume*
  [[nodiscard]] cpr::Response set_device_volume(volume volume, const std::string& device_id = "");

  // Sets the volume of currently playing device to *volume*
  [[nodiscard]] cpr::Response set_volume(volume volume);

  // Returns the volume percent of the current playing device
  std::optional<volume> get_current_playing_volume();

  // Returns the ID of the first desktop device found
  [[nodiscard]] std::optional<std::string> get_desktop_player_id();

  static void print_error_message(const cpr::Response& response);

private:
  [[nodiscard]] std::string get_token();

  token_t m_token_info;
  Config m_config;
};

}  // namespace spotify_volume_controller
