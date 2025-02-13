#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>

#include "oauth.h"

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include "windows_helpers.h"

// for convenience
using json = nlohmann::json;

namespace spotify_volume_controller::oauth
{

// Based on https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
static constexpr std::string_view b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  //=
static constexpr std::string base64_encode(const std::string_view in)
{
  std::string out;

  int val = 0, val_b = -6;
  for (unsigned char c : in) {
    val = (val << 8) + c;
    val_b += 8;
    while (val_b >= 0) {
      out.push_back(b[(val >> val_b) & 0x3F]);
      val_b -= 6;
    }
  }
  if (val_b > -6)
    out.push_back(b[((val << 8) >> (val_b + 8)) & 0x3F]);
  while (out.size() % 4)
    out.push_back('=');
  return out;
}

void open_uri(const std::string_view uri)
{
  std::stringstream ss;
  // TODO Implement Linux support
  ss << "cmd /c start \"\" \"";
  ss << uri;
  ss << "\"";

  // Opens the uri with the std browser on windows
  system(ss.str().c_str());
}

[[nodiscard]] std::string get_authorization_code(const std::string& callback_address, const Config& config)
{
  cpr::CurlHolder c {};
  cpr::Parameters params = {{"client_id", config.get_client_id()},
                            {"redirect_uri", config.get_redirect_url()},
                            {"response_type", "code"},
                            {"scope", std::string(SCOPES)},
                            {"show_dialog", "true"}};

  std::string auth_url = fmt::format("{}?{}", AUTHORIZATION_URL, params.GetContent(c));
  std::cout << "Please accept the prompt in your browser." << std::endl;
  std::cout << "If your browser did not open, please go to the following link to accept the prompt:" << '\n';
  std::cout << auth_url << std::endl;
  open_uri(auth_url);
  std::string authorization_code {};
  {
    std::thread th;
    httplib::Server server;
    server.Get("/callback",
              [&](const httplib::Request& req, httplib::Response res)
              {
                if (req.has_param("code")) {
                  authorization_code = req.get_param_value("code");
                  res.set_content("Successfully authenticated", "text/plain");
                } else if (req.has_param("error")) {
                  std::cerr << "Failed to get authorization code: " << req.get_param_value("error") << std::endl;
                  res.set_content("Failed to authenticate, check logs", "text/plain");
                } else {
                  std::cerr << "Failed to get authorization code: Unkown error" << std::endl;
                  res.set_content("Failed to authenticate, check logs", "text/plain");
                }
                // Based on https://github.com/yhirose/cpp-httplib/blob/master/example/one_time_request.cc
                th = std::thread([&]() { server.stop(); });
              });
    server.listen("0.0.0.0", 5000);
    th.join();
  }
  return authorization_code;
}

[[nodiscard]] json fetch_token(const std::string& code, const std::string& grant_type, const Config& config)
{
  cpr::Payload payload {{"grant_type", grant_type}};
  if (grant_type == "authorization_code") {
    payload.Add({"code", code});
    payload.Add({"redirect_uri", config.get_redirect_url()});
  } else {
    payload.Add({"refresh_token", code});
  }

  cpr::Response response = cpr::Post(
      cpr::Url {fmt::format("{}/{}", BASE_AUTHENTICATION_API_URL, TOKEN_API_ENDPOINT)},
      payload,
      cpr::Header {
          {"Authorization",
           fmt::format("Basic {}",
                       base64_encode(fmt::format("{}:{}", config.get_client_id(), config.get_client_secret())))}});
  return json::parse(response.text);
}

[[nodiscard]] token_t get_token(const std::string& authorization_code, const Config& config)
{
  json j_token = fetch_token(authorization_code, "authorization_code", config);
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
  std::cout << "Getting authorization token..." << std::endl;
  std::optional<token_t> token = read_token(config.config_directory());
  if (!token.has_value()) {
    std::cout << "No existing token found, creating new" << std::endl;
    std::string authorization_code = get_authorization_code(config.get_redirect_url(), config);
    if (authorization_code.empty()) {
      return {};
    }
    return get_token(authorization_code, config);
  } else if (token_is_expired(token.value())) {
    token = refresh_token(token.value(), config);
  }
  return token;
}

[[nodiscard]] token_t refresh_token(const token_t token, const Config& config)
{
  json j_new_token = fetch_token(token.refresh_token, "refresh_token", config);
  if (j_new_token.contains("error")) {
    std::cout << "Failed to refresh token." << std::endl;
    std::cout << "Error: " << j_new_token["error"] << std::endl;
    std::cout << "Error description: " << j_new_token["error_description"] << std::endl;
    if (j_new_token["error"].template get<std::string>() == "invalid_client") {
      std::cout << "Check that the client id/secret is correct." << std::endl;
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

void save_token(const token_t token, const std::filesystem::path& token_directory)
{
  std::ofstream token_file(token_directory / TOKEN_FILE_NAME);
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
  std::ifstream token_file(token_directory / TOKEN_FILE_NAME);

  if (token_file) {
    return {json::parse(token_file)};
  } else {
    std::cerr << "Failed to read token from file"
              << "\n";
    return {};
  }
}

[[nodiscard]] bool token_is_expired(const token_t token)
{
  std::chrono::duration time_since_epoch =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  return time_since_epoch > std::chrono::seconds(token.expires_at);
}

}  // namespace spotify_volume_controller::oauth
