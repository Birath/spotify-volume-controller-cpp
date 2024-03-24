#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <ranges>
#include <string>

#include "oauth.h"

#include "SpotifyListener.h"
#include "http_utils.h"
#include "windows_helpers.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility;

namespace spotify_volume_controller::oauth
{

void open_uri(const uri& uri)
{
  std::stringstream ss;
  // TODO Implement Linux support
  ss << "cmd /c start \"\" \"";
  // Converts wstring to string
  ss << windows::wide_string_to_string(uri.to_string());
  ss << "\"";

  // Opens the uri with the std browser on windows
  system(ss.str().c_str());
}

utility::string_t get_authorization_code(const uri& callback_address, const Config& config)
{
  SpotifyListener listener(callback_address);
  listener.open();

  web::uri uri = create_authorization_uri(config);
  std::wcout << "Please accept the prompt in your browser." << std::endl;
  std::wcout << "If your browser did not open, please go to the following link to accept the prompt:" << '\n';
  std::wcout << uri.to_string() << std::endl;
  open_uri(uri);

  utility::string_t authorization_code = listener.get_authorization_code();
  listener.close();
  if (authorization_code.substr(0, 5) == L"Error") {
    std::wcout << "Authorization failed. Reason: " << authorization_code << std::endl;
    return L"";
  }
  return authorization_code;
}

web::json::value fetch_token(const utility::string_t& code, const utility::string_t& grant_type, const Config& config)
{
  http::http_request token_request = create_token_request(code, grant_type, config);

  pplx::task<http_response> response_task = request(token_request, Config::BASE_AUTHENTICATION_API_URI);
  response_task.wait();
  http_response response = response_task.get();
  web::json::value token = get_json_response_body(response);
  return token;
}

web::json::value get_token(const utility::string_t& authorization_code, const Config& config)
{
  web::json::value token = fetch_token(authorization_code, L"authorization_code", config);

  // Adds the epoch time when the token will expire
  auto expiration_time = std::chrono::duration_cast<seconds>(std::chrono::system_clock::now().time_since_epoch());
  expiration_time += std::chrono::seconds(token[L"expires_in"].as_integer());
  token[L"expires_at"] = expiration_time.count();
  save_token(token, config.config_directory());
  return token;
}

web::json::value get_token(const Config& config)
{
  std::wcout << "Getting authorization token..." << std::endl;
  web::json::value token = read_token(config.config_directory());
  if (token.is_null()) {
    std::wcout << "No existing token found, creating new" << std::endl;
    utility::string_t authorization_code = get_authorization_code(config.get_redirect_url(), config);
    if (authorization_code.empty()) {
      return json::value::null();
    }
    return get_token(authorization_code, config);
  } else if (token_is_expired(token)) {
    refresh_token(token, config);
  }
  return token;
}

void refresh_token(web::json::value& token, const Config& config)
{
  web::json::value new_token = fetch_token(token.at(L"refresh_token").as_string(), L"refresh_token", config);
  if (new_token.has_field(L"error")) {
    std::wcout << L"Failed to refresh token." << std::endl;
    std::wcout << L"Error: " << new_token[L"error"] << std::endl;
    std::wcout << L"Error description: " << new_token[L"error_description"] << std::endl;
    if (new_token[L"error"].as_string().compare(L"invalid_client") == 0) {
      std::wcout << L"Check that the client id/secret is correct." << std::endl;
    }
    std::cin.get();
    exit(1);
    return;
  }
  token[L"access_token"] = new_token.at(L"access_token");
  token[L"token_type"] = new_token.at(L"token_type");
  token[L"scope"] = new_token.at(L"scope");
  token[L"expires_in"] = new_token.at(L"expires_in");

  // Adds the epoch time when the token will expire
  auto expiration_time = std::chrono::duration_cast<seconds>(std::chrono::system_clock::now().time_since_epoch());
  expiration_time += std::chrono::seconds(token[L"expires_in"].as_integer());
  token[L"expires_at"] = expiration_time.count();
  save_token(token, config.config_directory());
}

void save_token(const web::json::value& token, const std::filesystem::path& token_directory)
{
  std::ofstream token_file(token_directory / TOKEN_FILE_NAME);
  if (token_file) {
    token.serialize(token_file);
    token_file.close();
  } else {
    std::cerr << "Failed to write token to file"
              << "\n";
  }
}

web::json::value read_token(const std::filesystem::path& token_directory)
{
  utility::ifstream_t token_file(token_directory / TOKEN_FILE_NAME);

  if (token_file) {
    web::json::value token = json::value::parse(token_file);
    return token;
  } else {
    std::cerr << "Failed to read token from file"
              << "\n";
    return json::value::null();
  }
}

bool token_is_expired(const web::json::value& token)
{
  uint64_t expiration_epoch_time = token.at(L"expires_at").as_number().to_uint64();
  std::chrono::duration time_since_epoch =
      std::chrono::duration_cast<seconds>(std::chrono::system_clock::now().time_since_epoch());
  return time_since_epoch > std::chrono::seconds(expiration_epoch_time);
}

http::http_request create_token_request(const utility::string_t& authorization_code,
                                        const utility::string_t& grant_type,
                                        const Config& config)
{
  http_request token_request;
  token_request.set_request_uri(BASE_TOKEN_URI);
  utility::string_t body = L"grant_type=" + grant_type;
  if (grant_type == L"authorization_code") {
    body.append(L"&code=" + authorization_code);
    body.append(L"&redirect_uri=" + config.get_redirect_url());
  } else {
    body.append(L"&refresh_token=" + authorization_code);
  }

  token_request.set_body(body);

  token_request.headers().set_content_type(L"application/x-www-form-urlencoded");
  token_request.headers().add(L"Authorization", L"Basic " + get_authorize_string(config));
  token_request.set_method(methods::POST);
  return token_request;
}

utility::string_t get_authorize_string(const Config& config)
{
  utility::string_t unencoded;
  unencoded.append(config.get_client_id() + L":" + config.get_client_secret());
  const std::string bytes = windows::wide_string_to_string(unencoded);
  std::vector<unsigned char> byte_vector;
  std::ranges::copy(
      std::views::transform(unencoded, [](const utility::char_t x) { return static_cast<unsigned char>(x); }),
      std::back_inserter(byte_vector));
  return utility::conversions::to_base64(byte_vector);
}

web::uri create_authorization_uri(const Config& config)
{
  web::uri_builder authorization_uri(BASE_AUTHORIZATION_URI);
  authorization_uri.append_query(L"client_id", config.get_client_id(), true);
  authorization_uri.append_query(L"redirect_uri", config.get_redirect_url(), true);
  authorization_uri.append_query(L"response_type", L"code", true);
  authorization_uri.append_query(L"scope", Config::SCOPES, true);
  authorization_uri.append_query(L"show_dialog", "true");
  return authorization_uri.to_uri();
}

}  // namespace spotify_volume_controller::oauth
