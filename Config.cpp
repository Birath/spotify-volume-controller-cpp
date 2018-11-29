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

bool Config::should_print_keys() const {
	return config.at(L"print_keys").as_bool();
}

int Config::get_volume_up() const {
	if (!config.has_field(L"volume_up")) {
		return -1;
	}
	web::json::value v_up = config.at(L"volume_up");
	if (!v_up.is_integer()) {
		return -1;
	}
	return v_up.as_integer();
}

int Config::get_volume_down() const {
	if (!config.has_field(L"volume_down")) {
		return -1;
	}
	web::json::value v_down = config.at(L"volume_down");
	
	if (!v_down.is_integer()) {
		return -1;
	}
	return v_down.as_integer();
}

bool Config::is_valid() {
	return !config.is_null();
}

const utility::string_t Config::SCOPES = L"user-read-playback-state user-modify-playback-state";

const web::uri Config::BASE_AUTHENTICATION_API_URI(L"https://accounts.spotify.com/api/");
