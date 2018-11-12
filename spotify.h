#pragma once
#include "cpprest/oauth2.h"
#include "cpprest/http_client.h"
#include "cpprest//http_listener.h"
#include <cpprest/filestream.h>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>

#include "SpotifyListener.h"
#include "Config.h"
#include "HttpUtils.h"

using namespace web::http;
using namespace web;



pplx::task<void> request(const web::uri &address, const method http_method = methods::GET);
pplx::task<http_response> request(http_request &request, const web::uri &address);

std::wstring get_authorization_code(const web::uri &callback_address);

web::json::value get_token(const std::wstring &authorization_code);

void open_uri(const web::uri &uri) ;

web::uri create_authorization_uri();
/// <summary>
/// Creates the request to the token endpoint using authorization_code
/// </summary>
http::http_request create_token_request(const utility::string_t &authorization_code);

/// <summary>
/// Returns a base64 encoded string of the string: <CLIENT_ID:CLIENT_SECRET> 
/// </summary>
utility::string_t get_authorize_string();

const web::uri BASE_AUTHORIZATION_URI(L"https://accounts.spotify.com/authorize");
const web::uri BASE_TOKEN_URI(L"/token");






