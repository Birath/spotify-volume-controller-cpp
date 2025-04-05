#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <nlohmann/json_fwd.hpp>

#include "Config.h"
#include "data_types.h"

namespace spotify_volume_controller::oauth
{

enum class grant_type : std::uint8_t
{
  authorization_code,
  refresh_token,
};

constexpr std::string_view scopes = "user-read-playback-state user-modify-playback-state";
constexpr std::string_view authorization_url = "https://accounts.spotify.com/authorize";
constexpr std::string_view base_authentication_api_url = "https://accounts.spotify.com/api";
constexpr std::string_view token_api_endpoint = "token";
constexpr std::string_view token_file_name = ".spotifytoken";

[[nodiscard]] auto get_authorization_code(const std::string& callback_address, const Config& config)
    -> std::optional<std::string>;

[[nodiscard]] auto get_token(const std::string& authorization_code, const Config& config) -> token_t;
[[nodiscard]] auto get_token(const Config& config) -> std::optional<token_t>;

[[nodiscard]] auto refresh_token(const token_t& token, const Config& config) -> token_t;

[[nodiscard]] auto fetch_token(const std::string& code, grant_type grant_type, const Config& config) -> nlohmann::json;

// Saves *token* to a file
void save_token(const token_t& token, const std::filesystem::path& token_directory);

// Returns the token stored on the disk
[[nodiscard]] auto read_token(const std::filesystem::path& token_directory) -> std::optional<token_t>;

[[nodiscard]] auto token_is_expired(const token_t& token) -> bool;

void open_uri(std::string_view uri);

}  // namespace spotify_volume_controller::oauth
