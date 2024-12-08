#pragma once
#include <cpprest/http_client.h>
namespace spotify_volume_controller
{

/// <summary>
/// Gets the body from a http response. Only works if the the body follows the json format
/// </summary>
web::json::value get_json_response_body(const web::http::http_response& response);
/// <summary>
/// Sends a http request to *address* using http method *http_method* (GET if none provided). Returns the request task.
/// </summary>
pplx::task<web::http::http_response> request(const web::uri& address,
                                             const web::http::method& http_method = web::http::methods::GET);

/// <summary>
/// Sends a web request to *address*. Returns the request task.
/// </summary>
pplx::task<web::http::http_response> request(web::http::http_request& request, const web::uri& address);
}  // namespace spotify_volume_controller