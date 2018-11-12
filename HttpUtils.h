#pragma once
#include <cpprest/http_client.h>

using namespace web;

/// <summary>
/// Gets the body from a http response. Only works if the the body follows the json format
/// </summary>
json::value get_json_response_body(const http::http_response &response);


