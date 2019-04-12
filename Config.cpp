#include "Config.h"
#include <iostream>
Config::Config() {
	utility::ifstream_t config_file("config.json");
	if (config_file) {
		config = web::json::value::parse(config_file);
	}
	else {
		using namespace std;
		using namespace web;
		cout << "No config file found, creating." << endl;
		utility::ofstream_t config_file("config.json");
		utility::string_t input;

		get_user_input(L"Please enter your spotify client id", input, true);
		config[L"client_id"] = json::value::string(input);
		
		get_user_input(L"Please enter your spotify client secret", input, true);
		config[L"client_secret"] = json::value::string(input);

		get_user_input(L"Enter callback url, or leave empty for default (" + Config::DEFAULT_CALLBACK_URL + L").", input);
		if (input.empty()) {
			config[L"rediriect_url"] = json::value::string(Config::DEFAULT_CALLBACK_URL);
		}
		else {
			config[L"rediriect_url"] = json::value::string(input);
		}

		get_user_input(L"Enter volume up virtual keycode as a number, or leave empty for default", input);
		if (input.empty()) {
			config[L"volume_up"] = json::value::string(L"default");
		}
		else {
			config[L"volume_up"] = json::value::string(input);
		}
		get_user_input(L"Enter volume down virtual keycode as a number, or leave empty for default", input);
		if (input.empty()) {
			config[L"volume_down"] = json::value::string(L"default");
		}
		else {
			config[L"volume_down"] = json::value::string(input);
		}
		config[L"print_keys"] = json::value::boolean(false);

		config_file << config.serialize();
		config_file.close();
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

bool Config::is_default_down() const {
	if (!config.has_field(L"volume_down")) {
		return false;
	}
	web::json::value v_down = config.at(L"volume_down");
	return v_down.is_string() && v_down.as_string().compare(L"default") == 0;
}

bool Config::is_default_up() const {
	if (!config.has_field(L"volume_up")) {
		return false;
	}
	web::json::value v_up = config.at(L"volume_up");
	return v_up.is_string() && v_up.as_string().compare(L"default") == 0;
}

bool Config::is_valid() {
	return !config.is_null();
}

void Config::get_user_input(utility::string_t prompt, utility::string_t &input, bool not_empty) const {
	using namespace std;
	input.clear();
	wcout << prompt << endl;
	getline(wcin, input);
	while (not_empty && input.empty()) {
		cout << "Input can't be empty" << endl;
		getline(wcin, input);
	}
}


const utility::string_t Config::SCOPES = L"user-read-playback-state user-modify-playback-state";

const web::uri Config::BASE_AUTHENTICATION_API_URI(L"https://accounts.spotify.com/api/");
