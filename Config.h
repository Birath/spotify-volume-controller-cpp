#pragma once
#include <cpprest/uri.h>
#include <cpprest/json.h>


class Config
{
public:
	Config();

	~Config();

	utility::string_t get_client_id() const;
	utility::string_t get_client_secret() const;
	utility::string_t get_redirect_url() const;
	int get_volume_up() const;
	int get_volume_down() const;
	bool should_print_keys() const;
	bool is_valid();

	static const utility::string_t SCOPES;
	static const web::uri BASE_AUTHENTICATION_API_URI;
private:
	web::json::value config;
};

