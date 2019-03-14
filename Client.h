#pragma once

#include <cpprest/http_client.h>
#include <map>

#include "Config.h"

using namespace web;

class Client {
public:
	Client(json::value &token_info, const Config& m_config);
	~Client();
	/// <summary>
	/// Makes a http request to *endpoint* using http method *http_method*
	/// </summary>
	http::http_response api_request(const utility::string_t &endpoint, const http::method http_method);
	http::http_response api_request(const utility::string_t &endpoint, const http::method http_method, const utility::string_t &query );

	/// <summary>
	/// Makes a request to the device endpoint
	/// </summary>
	json::value get_device_info();

	/// <summary>
	/// Sets the volume of device_id to *volume* 
	/// </summary>
	bool set_device_volume(int volume, const utility::string_t &device_id = L"");

	/// <summary>
	/// Sets the volume of currently playing device to *volume* 
	/// </summary>
	bool set_volume(int volume);

	/// <summary>
	/// Returns the volume percent of the current playing device, or -1 if no playing devices
	/// </summary>
	int get_current_playing_volume();

	/// <summary>
	/// Returns the first desktop device found
	/// </summary>
	json::value get_desktop_player();

private:

	void authorize_header(http::http_request request);
	void print_error_message(const http::http_response &response);
	void print_play_error_message(const http::http_response &response);

	json::value m_token_info;
	const Config m_config;
	const utility::string_t ACCESS_TOKEN = L"access_token";
	static const uri BASE_API_URI;
};

