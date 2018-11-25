#pragma once
#include <cpprest/http_client.h>

/// <summary>
/// Gets the body from a http response. Only works if the the body follows the json format
/// </summary>
pplx::task<web::json::value> get_json_response_body(const web::http::http_response &response);

pplx::task<web::http::http_response> request(const web::uri &address, const web::http::method http_method = web::http::methods::GET);
pplx::task<web::http::http_response> request(web::http::http_request &request, const web::uri &address);