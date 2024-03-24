#pragma once
#include <cpprest/http_listener.h>
namespace spotify_volume_controller
{

class SpotifyListener
{
public:
  SpotifyListener(const web::uri& address);
  SpotifyListener();
  ~SpotifyListener();

  /// <summary>
  /// Gets the authorization code when it's done or, if it failed, the reason why
  /// </summary>
  /// <returns>The authorization code or error message</returns>
  utility::string_t get_authorization_code();

  void open();
  void close();

private:
  void request_handler(web::http::http_request& request);

  web::http::experimental::listener::http_listener m_server;
  utility::stringstream_t m_authorization_code;
  bool closed = false;
};

}  // namespace spotify_volume_controller