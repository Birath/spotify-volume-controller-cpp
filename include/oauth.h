#pragma once
#include <cpprest/oauth2.h>
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/filestream.h>

#include "Config.h"


utility::string_t get_authorization_code(const web::uri &callback_address, const Config &config);

web::json::value get_token(const utility::string_t &authorization_code, const Config &config);
web::json::value get_token(const Config &config);

void refresh_token(web::json::value &token, const Config &config);


web::json::value fetch_token(const utility::string_t &code, const utility::string_t &grant_type, const Config &config);

// Saves *token* to a file
void save_token(const web::json::value &token, const std::filesystem::path token_directory);

// Returns the token stored on the disk
web::json::value read_token(const std::filesystem::path token_directory);

bool token_is_expired(const web::json::value &token);

void open_uri(const web::uri &uri) ;

web::uri create_authorization_uri(const Config& config);

/// <summary>
/// Creates the request to the token endpoint using authorization_code
/// </summary>
web::http::http_request create_token_request(const utility::string_t &authorization_code, const utility::string_t &grant_type, const Config &config);

/// <summary>
/// Returns a base64 encoded string of the string: CLIENT_ID:CLIENT_SECRET
/// </summary>
utility::string_t get_authorize_string(const Config &config);

const web::uri BASE_AUTHORIZATION_URI(L"https://accounts.spotify.com/authorize");
const web::uri BASE_TOKEN_URI(L"/token");
const utility::string_t TOKEN_FILE_NAME = L".spotifytoken";






