#pragma once
#include <cpprest/uri.h>

class Config
{
public:
	Config();
	~Config();

	static const utility::string_t CLIENT_ID;
	static const utility::string_t CLIENT_SECRET;
	static const utility::string_t SCOPES;
	
	static const utility::string_t REDIRECT_URI;

	static const web::uri BASE_AUTHENTICATION_API_URI;

};

