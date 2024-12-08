#pragma once

#include <map>

#include <fmt/format.h>
// fmt needs to be included before cpprest for now
#include <cpprest/http_client.h>
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
  Client(web::json::value& token_info, const Config& m_config);
  /// <summary>
  /// Makes a http request to *endpoint* using http method *http_method*
  /// </summary>
  cpr::Response api_request(const std::string_view endpoint, const web::http::method http_method);
  [[nodiscard]] cpr::Response put_api_request(const std::string_view endpoint, const cpr::Payload& parameters);

  /// <summary>
  /// Makes a request to the device endpoint
  /// </summary>
  [[nodiscard]] std::optional<json> get_devices();

  [[nodiscard]] std::optional<volume> get_device_volume(const std::string_view id);

  /// <summary>
  /// Sets the volume of device_id to *volume*
  /// </summary>
  [[nodiscard]] cpr::Response set_device_volume(volume volume, const std::string& device_id = "");

  /// <summary>
  /// Sets the volume of currently playing device to *volume*
  /// </summary>
  [[nodiscard]] cpr::Response set_volume(volume volume);

  /// <summary>
  /// Returns the volume percent of the current playing device, or -1 if no playing devices
  /// </summary>
  std::optional<volume> get_current_playing_volume();

  /// <summary>
  /// Returns the ID of the first desktop device found
  /// </summary>
  [[nodiscard]] std::optional<std::string> get_desktop_player_id();

  void print_error_message(const cpr::Response& response) const;

private:
  [[nodiscard]] std::string get_token();

  template<typename WideCharT>
  std::string convert_UTF16_to_UTF8(const std::basic_string<WideCharT>& wstr) const
  {
    const wchar_t* wstr_ptr = reinterpret_cast<const wchar_t*>(wstr.c_str());
    int wstr_len = static_cast<int>(wstr.size());
    int str_len = WideCharToMultiByte(CP_UTF8, 0, wstr_ptr, wstr_len, nullptr, 0, nullptr, nullptr);
    std::string str(str_len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr_ptr, wstr_len, str.data(), str_len, nullptr, nullptr);
    return str;
  }

  [[nodiscard]] std::string convert_to_string(const utility::string_t& w_str) const
  {
    return convert_UTF16_to_UTF8(w_str);
  }

  web::json::value m_token_info;
  const Config m_config;
  const utility::string_t ACCESS_TOKEN = L"access_token";
};

}  // namespace spotify_volume_controller
