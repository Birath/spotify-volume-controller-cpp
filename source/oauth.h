#pragma once
#include <string_view>

#include <nlohmann/json.hpp>

#include "Config.h"

namespace spotify_volume_controller::oauth
{

constexpr std::string_view SCOPES = "user-read-playback-state user-modify-playback-state";
constexpr std::string_view AUTHORIZATION_URL = "https://accounts.spotify.com/authorize";
constexpr std::string_view BASE_AUTHENTICATION_API_URL = "https://accounts.spotify.com/api";
constexpr std::string_view TOKEN_API_ENDPOINT = "token";
constexpr std::string_view TOKEN_FILE_NAME = ".spotifytoken";

[[nodiscard]] std::string get_authorization_code(const std::string& callback_address, const Config& config);

[[nodiscard]] token_t get_token(const std::string& authorization_code, const Config& config);
[[nodiscard]] std::optional<token_t> get_token(const Config& config);

[[nodiscard]] token_t refresh_token(const token_t token, const Config& config);

[[nodiscard]] nlohmann::json fetch_token(const std::string& code, const std::string& grant_type, const Config& config);

// Saves *token* to a file
void save_token(const token_t token, const std::filesystem::path& token_directory);

// Returns the token stored on the disk
[[nodiscard]] std::optional<token_t> read_token(const std::filesystem::path& token_directory);

[[nodiscard]] bool token_is_expired(const token_t token);

void open_uri(const std::string_view uri);

}  // namespace spotify_volume_controller::oauth
