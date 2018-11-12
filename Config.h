#pragma once
#include <string>
#include <vector>
#include <cpprest/asyncrt_utils.h>

class Config
{
public:
	Config();
	~Config();

	static utility::string_t CLIENT_ID;
	static utility::string_t CLIENT_SECRET;
	static utility::string_t SCOPES;
	
	static utility::string_t REDIRECT_URI;

};

