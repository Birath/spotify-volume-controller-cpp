#pragma once

#include <cpprest/http_client.h>
#include "Config.h"

using namespace web;

class Client
{
public:
	Client(json::value &token_info);
	~Client();
	/// <summary>
	/// Makes a http request to *endpoint* using http method *http_method*
	/// </summary>
	pplx::task<http::http_response> api_request(const utility::string_t &endpoint, const http::method http_method);
	pplx::task<http::http_response> api_request(const utility::string_t &endpoint, const json::value data, const http::method http_method);
	/// <summary>
	/// Makes a request to the device endpoint
	/// </summary>
	pplx::task<json::value> get_device_info();

private:

	void authorize_header(http::http_request request);

	json::value m_token_info;
	const utility::string_t ACCESS_TOKEN = L"access_token";
	const uri BASE_API_URI;
};

