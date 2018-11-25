#pragma once
#include "cpprest/oauth2.h"
#include "cpprest/http_client.h"
#include "cpprest//http_listener.h"
#include <cpprest/filestream.h>

utility::string_t get_authorization_code(const web::uri &callback_address);

web::json::value get_token(const utility::string_t &authorization_code);
web::json::value get_token();

void refresh_token(web::json::value &token);
// Saves *token* to a file
void save_token(const web::json::value &token);

// Returns the token stored on the disk
web::json::value read_token();

bool token_is_expired(const web::json::value &token);

void open_uri(const web::uri &uri) ;

web::uri create_authorization_uri();

/// <summary>
/// Creates the request to the token endpoint using authorization_code
/// </summary>
web::http::http_request create_token_request(const utility::string_t &authorization_code, const utility::string_t &grant_type);

/// <summary>
/// Returns a base64 encoded string of the string: <CLIENT_ID:CLIENT_SECRET> 
/// </summary>
utility::string_t get_authorize_string();

const web::uri BASE_AUTHORIZATION_URI(L"https://accounts.spotify.com/authorize");
const web::uri BASE_TOKEN_URI(L"/token");
const utility::string_t TOKEN_FILE_NAME = L".spotifytoken";






