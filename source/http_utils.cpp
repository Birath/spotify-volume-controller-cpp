
#include "http_utils.h"

#include <cpprest/base_uri.h>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>
#include <ppltasks.h>

namespace spotify_volume_controller
{

auto get_json_response_body(const web::http::http_response& response) -> web::json::value
{
  return response.extract_json().get();
}

auto request(const web::uri& address, const web::http::method& http_method) -> pplx::task<web::http::http_response>
{
  web::http::client::http_client client(address);
  return client.request(http_method).then([](web::http::http_response response) { return response; });
}

auto request(web::http::http_request& request, const web::uri& address) -> pplx::task<web::http::http_response>
{
  web::http::client::http_client client(address);
  return client.request(request).then([](web::http::http_response response) { return response; });
}
}  // namespace spotify_volume_controller