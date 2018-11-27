#pragma once
#include <cpprest/uri.h>
#include <cpprest/json.h>


class Config
{
public:
	static Config& get_instance() {
		static Config instance;
		return instance;
	}
	Config();

	~Config();

	//Config(Config const&) = delete;
	void operator=(Config const&) = delete;

	utility::string_t get_client_id() const;
	utility::string_t get_client_secret() const;
	utility::string_t get_redirect_url() const;
	bool is_valid();

	static const utility::string_t SCOPES;
	static const web::uri BASE_AUTHENTICATION_API_URI;
private:
	web::json::value config;
};

