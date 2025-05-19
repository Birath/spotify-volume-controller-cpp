#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include "oauth.h"

#include <cpr/api.h>
#include <cpr/cprtypes.h>
#include <cpr/curlholder.h>
#include <cpr/parameters.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <curl/curl.h>
#include <curl/urlapi.h>
#include <fmt/core.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Config.h"
#include "data_types.h"

namespace spotify_volume_controller::oauth
{
// for convenience
using json = nlohmann::json;

namespace
{

constexpr std::string grant_type_to_string(grant_type grant_type)
{
  switch (grant_type) {
    case grant_type::authorization_code:
      return "authorization_code";
    case grant_type::refresh_token:
      return "refresh_token";
  }
  std::abort();
}

// Based on https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
constexpr std::string_view base_64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  //=
constexpr std::string base64_encode(const std::string_view input)
{
  std::string out;

  int val = 0;
  int val_b = -6;
  for (unsigned char const c : input) {
    val = (val << 8) + c;
    val_b += 8;
    while (val_b >= 0) {
      out.push_back(base_64_alphabet[(val >> val_b) & 0x3F]);
      val_b -= 6;
    }
  }
  if (val_b > -6) {
    out.push_back(base_64_alphabet[((val << 8) >> (val_b + 8)) & 0x3F]);
  }
  while ((out.size() % 4) != 0U) {
    out.push_back('=');
  }
  return out;
}

}  // namespace

void open_uri(const std::string_view uri)
{
  std::stringstream ss;
  // TODO Implement Linux support
  ss << R"(cmd /c start "" ")";
  ss << uri;
  ss << "\"";

  // Opens the uri with the std browser on windows
  system(ss.str().c_str());
}

[[nodiscard]] std::optional<std::string> get_authorization_code(const std::string& callback_address,
                                                                const Config& config)
{
  cpr::CurlHolder const curl_holder {};
  cpr::Parameters const params = {{"client_id", config.get_client_id()},
                                  {"redirect_uri", config.get_redirect_url()},
                                  {"response_type", "code"},
                                  {"scope", std::string(scopes)},
                                  {"show_dialog", "true"}};

  std::string const auth_url = fmt::format("{}?{}", authorization_url, params.GetContent(curl_holder));
  std::cout << "Please accept the prompt in your browser." << '\n';
  std::cout << "If your browser did not open, please go to the following link to accept the prompt:" << '\n';
  std::cout << auth_url << '\n';
  open_uri(auth_url);
  std::string host {};
  int port = 0;
  std::string part;
  {
    CURLU* url_handle = curl_url();
    CURLUcode curl_return_code = curl_url_set(url_handle, CURLUPART_URL, callback_address.c_str(), 0);
    if (curl_return_code == CURLUE_OK) {
      char* host_part = nullptr;
      curl_return_code = curl_url_get(url_handle, CURLUPART_HOST, &host_part, 0);
      if (curl_return_code != CURLUE_OK) {
        fmt::println(stderr, "Failed to parse callback URL: {}", curl_url_strerror(curl_return_code));
        curl_url_cleanup(url_handle);
        return {};
      }
      host.assign(host_part);
      curl_free(host_part);

      char* port_part = nullptr;
      curl_return_code = curl_url_get(url_handle, CURLUPART_PORT, &port_part, 0);
      if (curl_return_code != CURLUE_OK) {
        fmt::println(stderr, "Failed to parse callback URL: {}", curl_url_strerror(curl_return_code));
        curl_url_cleanup(url_handle);
        return {};
      }
      std::string port_str {port_part};
      curl_free(port_part);
      try {
        port = std::stoi(port_str);
      } catch (std::invalid_argument&) {
        fmt::println(stderr, "Failed to convert port {} to integer from callback URL", port_str);
        curl_url_cleanup(url_handle);
        return {};
      }
      char* part_part = nullptr;
      curl_return_code = curl_url_get(url_handle, CURLUPART_PATH, &part_part, 0);
      if (curl_return_code != CURLUE_OK) {
        fmt::println(stderr, "Failed to parse callback URL: {}", curl_url_strerror(curl_return_code));
        curl_url_cleanup(url_handle);
        return {};
      }
      part.assign(part_part);
      curl_free(part_part);
    } else {
      fmt::println(stderr, "Failed to parse callback URL: {}", curl_url_strerror(curl_return_code));
      curl_url_cleanup(url_handle);
      return {};
    }
    curl_url_cleanup(url_handle);
  }

  std::string authorization_code {};
  {
    std::thread stop_thread;
    httplib::Server server;
    server.Get(part,
               [&](const httplib::Request& req, httplib::Response res)
               {
                 if (req.has_param("code")) {
                   authorization_code = req.get_param_value("code");
                   res.set_content("Successfully authenticated", "text/plain");
                 } else if (req.has_param("error")) {
                   std::cerr << "Failed to get authorization code: " << req.get_param_value("error") << '\n';
                   res.set_content("Failed to authenticate, check logs", "text/plain");
                 } else {
                   std::cerr << "Failed to get authorization code: Unkown error" << '\n';
                   res.set_content("Failed to authenticate, check logs", "text/plain");
                 }
                 // Based on https://github.com/yhirose/cpp-httplib/blob/master/example/one_time_request.cc
                 stop_thread = std::thread([&]() { server.stop(); });
               });
    server.listen(host, port);
    stop_thread.join();
  }
  return authorization_code;
}

[[nodiscard]] json fetch_token(const std::string& code, grant_type grant_type, const Config& config)
{
  cpr::Payload payload {{"grant_type", grant_type_to_string(grant_type)}};
  switch (grant_type) {
    case grant_type::authorization_code:
      payload.Add({"code", code});
      payload.Add({"redirect_uri", config.get_redirect_url()});
      break;
    case grant_type::refresh_token:
      payload.Add({"refresh_token", code});
      break;
  }
  cpr::Response const response = cpr::Post(
      cpr::Url {fmt::format("{}/{}", base_authentication_api_url, token_api_endpoint)},
      payload,
      cpr::Header {
          {"Authorization",
           fmt::format("Basic {}",
                       base64_encode(fmt::format("{}:{}", config.get_client_id(), config.get_client_secret())))}});
  return json::parse(response.text);
}

[[nodiscard]] token_t get_token(const std::string& authorization_code, const Config& config)
{
  json const j_token = fetch_token(authorization_code, grant_type::authorization_code, config);
  if (j_token.contains("error")) {
    // TODO what to do here?
  }
  token_t token {j_token};

  // Adds the epoch time when the token will expire
  auto expiration_time =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  expiration_time += std::chrono::seconds(token.expires_in);
  token.expires_at = expiration_time.count();
  save_token(token, config.config_directory());
  return token;
}

[[nodiscard]] std::optional<token_t> get_token(const Config& config)
{
  std::cout << "Getting authorization token..." << '\n';
  std::optional<token_t> token = read_token(config.config_directory());
  if (!token.has_value()) {
    std::cout << "No existing token found, creating new" << '\n';
    std::optional<std::string> const authorization_code = get_authorization_code(config.get_redirect_url(), config);
    if (!authorization_code.has_value() || authorization_code->empty()) {
      return {};
    }
    return get_token(authorization_code.value(), config);
  }
  if (token_is_expired(token.value())) {
    token = refresh_token(token.value(), config);
  }
  return token;
}

[[nodiscard]] token_t refresh_token(const token_t& token, const Config& config)
{
  json j_new_token = fetch_token(token.refresh_token, grant_type::refresh_token, config);
  if (j_new_token.contains("error")) {
    std::cout << "Failed to refresh token." << '\n';
    std::cout << "Error: " << j_new_token["error"] << '\n';
    std::cout << "Error description: " << j_new_token["error_description"] << '\n';
    if (j_new_token["error"].template get<std::string>() == "invalid_client") {
      std::cout << "Check that the client id/secret is correct." << '\n';
    }
    std::cin.get();
    exit(1);
  }
  token_t new_token {j_new_token};
  // From https://developer.spotify.com/documentation/web-api/tutorials/refreshing-tokens
  // Depending on the grant used to get the initial refresh token, a refresh token might not be included in each
  // response. When a refresh token is not returned, continue using the existing token.
  if (new_token.refresh_token.empty()) {
    new_token.refresh_token = token.refresh_token;
  }
  // Adds the epoch time when the token will expire
  auto expiration_time =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  expiration_time += std::chrono::seconds(new_token.expires_in);
  new_token.expires_at = expiration_time.count();
  save_token(new_token, config.config_directory());
  return new_token;
}

void save_token(const token_t& token, const std::filesystem::path& token_directory)
{
  std::ofstream token_file(token_directory / token_file_name);
  if (token_file) {
    token_file << token.as_json();
    token_file.close();
  } else {
    std::cerr << "Failed to write token to file"
              << "\n";
  }
}

std::optional<token_t> read_token(const std::filesystem::path& token_directory)
{
  std::ifstream token_file(token_directory / token_file_name);

  if (token_file) {
    return {token_t(json::parse(token_file))};
  }
  std::cerr << "Failed to read token from file"
            << "\n";
  return {};
}

[[nodiscard]] bool token_is_expired(const token_t& token)
{
  auto const time_since_epoch =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  return time_since_epoch > std::chrono::seconds(token.expires_at);
}

}  // namespace spotify_volume_controller::oauth
