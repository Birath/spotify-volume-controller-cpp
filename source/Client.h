#pragma once

#include <map>

#include <fmt/format.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "Config.h"
#include "data_types.h"
// for convenience
using json = nlohmann::json;
namespace spotify_volume_controller
{

class Client
{
public:
  Client(token_t token_info, const Config& m_config);

  [[nodiscard]] cpr::Response api_request(const std::string_view endpoint);
  [[nodiscard]] cpr::Response put_api_request(const std::string_view endpoint, const cpr::Payload& parameters);


  // Makes a request to the device endpoint
  [[nodiscard]] std::optional<json> get_devices();

  [[nodiscard]] std::optional<volume> get_device_volume(const std::string_view id);

  // Sets the volume of device_id to *volume*
  [[nodiscard]] cpr::Response set_device_volume(volume volume, const std::string& device_id = "");

  // Sets the volume of currently playing device to *volume*
  [[nodiscard]] cpr::Response set_volume(volume volume);

  // Returns the volume percent of the current playing device
  std::optional<volume> get_current_playing_volume();

  // Returns the ID of the first desktop device found
  [[nodiscard]] std::optional<std::string> get_desktop_player_id();

  void print_error_message(const cpr::Response& response) const;

private:
  [[nodiscard]] std::string get_token();

  token_t m_token_info;
  const Config m_config;
};

}  // namespace spotify_volume_controller
