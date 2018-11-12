#pragma once
#include <string>
#include <vector>
#include <cpprest/uri.h>

using namespace web;
class Config
{
public:
	Config();
	~Config();

	static utility::string_t CLIENT_ID;
	static utility::string_t CLIENT_SECRET;
	static utility::string_t SCOPES;
	
	static utility::string_t REDIRECT_URI;

	static uri BASE_API_URI;

};

