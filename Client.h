#pragma once

#include <cpprest/http_client.h>
#include "Config.h"

using namespace web;

class Client
{
public:
	Client(json::value &token_info);
	~Client();

	pplx::task<http::http_response> api_request(const utility::string_t &endpoint, const http::method http_method);
	pplx::task<http::http_response> api_request(const utility::string_t &endpoint, const json::value data, const http::method http_method);

private:
	json::value m_token_info;
};

