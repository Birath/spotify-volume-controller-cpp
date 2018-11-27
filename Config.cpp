#include "Config.h"

Config::Config() {
	utility::ifstream_t config_file("config.json");
	if (config_file) {
		config = web::json::value::parse(config_file);
	}
}

Config::~Config() {
}

utility::string_t Config::get_client_id() const{
	return config.at(L"client_id").as_string();
}
utility::string_t Config::get_client_secret() const {
	return config.at(L"client_secret").as_string();

}
utility::string_t Config::get_redirect_url() const{
	return config.at(L"redirect_url").as_string();
}

bool Config::is_valid() {
	return !config.is_null();
}

const utility::string_t Config::SCOPES = L"user-read-playback-state user-modify-playback-state";

const web::uri Config::BASE_AUTHENTICATION_API_URI(L"https://accounts.spotify.com/api/");
