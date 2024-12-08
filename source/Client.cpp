#include "Client.h"

#include "http_utils.h"
#include "oauth.h"
namespace spotify_volume_controller
{

const web::uri Client::BASE_API_URI(L"https://api.spotify.com");

Client::Client(web::json::value& token_info, const Config& config)
    : m_token_info(token_info)
    , m_config(config)
{
  client = new web::http::client::http_client(BASE_API_URI);
}

Client::~Client()
{
  free(client);
}

web::http::http_response Client::api_request(const utility::string_t& endpoint, const web::http::method http_method)
{
  web::http::http_request request;
  request.set_request_uri(endpoint);
  request.set_method(http_method);
  authorize_header(request);
  return client->request(request).get();
}

web::http::http_response Client::api_request(const utility::string_t& endpoint,
                                             const web::http::method http_method,
                                             const utility::string_t& query)
{
  web::uri_builder api_uri(BASE_API_URI);
  api_uri.append_path(endpoint);
  api_uri.append_query(query, true);

  return api_request(api_uri.to_string(), http_method);
}

std::optional<web::json::array> Client::get_devices()
{
  try {
    web::http::http_response response = api_request(L"/v1/me/player/devices", web::http::methods::GET);
    if (response.status_code() != web::http::status_codes::OK) {
      print_error_message(response);
      return {};
    }
    return get_json_response_body(response).at(L"devices").as_array();
  } catch (web::http::http_exception e) {
    std::wcout << "Spotify API error:" << std::endl;
    std::cout << e.error_code().message() << std::endl;

    return {};
  }
}

std::optional<volume> Client::get_device_volume(const utility::string_t& id)
{
  std::optional<web::json::array> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device.has_string_field(L"id") && id == device[L"id"].as_string()) {
      return device.has_integer_field(L"volume_percent") ? device[L"volume_percent"].as_number().to_uint32()
                                                         : std::optional<volume> {};
    }
  }
  return {};
}

[[nodiscard]] web::http::http_response Client::set_device_volume(volume volume, const utility::string_t& device_id)
{
  utility::stringstream_t query;
  query << "volume_percent=" << volume;
  if (!device_id.empty()) {
    query << "&";
    query << "device_id=" << device_id;
  }

  return api_request(L"/v1/me/player/volume", web::http::methods::PUT, query.str());
}

[[nodiscard]] web::http::http_response Client::set_volume(volume volume)
{
  return set_device_volume(volume);
}

std::optional<volume> Client::get_current_playing_volume()
{
  std::optional<web::json::array> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device[L"is_active"].as_bool()) {
      // Volume percentage may be null according to documentation
      if (device[L"volume_percent"].is_integer())
        return device[L"volume_percent"].as_number().to_uint32();
    }
  }
  return {};
}

web::json::value Client::get_desktop_player()
{
  std::optional<web::json::array> devices = get_devices();
  if (!devices.has_value())
    return {};

  for (auto&& device : devices.value()) {
    if (device[L"type"].as_string().compare(L"Computer") == 0)
      return device;
  }
  return web::json::value::Null;
}

void Client::authorize_header(web::http::http_request request)
{
  if (oauth::token_is_expired(m_token_info)) {
    oauth::refresh_token(m_token_info, m_config);
  }
  request.headers()[L"Authorization"] = L"Bearer " + m_token_info[ACCESS_TOKEN].as_string();
}

void Client::print_error_message(const web::http::http_response& response)
{
  web::json::value body = get_json_response_body(response);
  web::json::value error_body = body[L"error"];
  std::wcerr << "Status code: " << error_body[L"status"] << '\n';
  std::wcerr << "Error message: " << error_body[L"message"] << '\n';
  if (error_body.has_string_field(L"reason")) {
    std::wcerr << "Reason: " << error_body[L"reason"];
  }
  std::wcerr << std::endl;
}

}  // namespace spotify_volume_controller
