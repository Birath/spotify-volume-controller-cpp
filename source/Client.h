#pragma once

#include <map>

#include <cpprest/http_client.h>

#include "Config.h"
#include "data_types.h"

namespace spotify_volume_controller
{

class Client
{
public:
  Client(web::json::value& token_info, const Config& m_config);
  ~Client();
  /// <summary>
  /// Makes a http request to *endpoint* using http method *http_method*
  /// </summary>
  web::http::http_response api_request(const utility::string_t& endpoint, const web::http::method http_method);
  web::http::http_response api_request(const utility::string_t& endpoint,
                                       const web::http::method http_method,
                                       const utility::string_t& query);

  /// <summary>
  /// Makes a request to the device endpoint
  /// </summary>
  std::optional<web::json::array> get_devices();

  std::optional<volume> get_device_volume(const utility::string_t& id);

  /// <summary>
  /// Sets the volume of device_id to *volume*
  /// </summary>
  [[nodiscard]] web::http::http_response set_device_volume(volume volume, const utility::string_t& device_id = L"");

  /// <summary>
  /// Sets the volume of currently playing device to *volume*
  /// </summary>
  [[nodiscard]] web::http::http_response set_volume(volume volume);

  /// <summary>
  /// Returns the volume percent of the current playing device, or -1 if no playing devices
  /// </summary>
  std::optional<volume> get_current_playing_volume();

  /// <summary>
  /// Returns the first desktop device found
  /// </summary>
  web::json::value get_desktop_player();

  static void print_error_message(const web::http::http_response& response);
private:
  void authorize_header(web::http::http_request request);

  web::json::value m_token_info;
  web::http::client::http_client* client;
  const Config m_config;
  const utility::string_t ACCESS_TOKEN = L"access_token";
  static const web::uri BASE_API_URI;
};

}  // namespace spotify_volume_controller
