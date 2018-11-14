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

	static const utility::string_t CLIENT_ID;
	static const utility::string_t CLIENT_SECRET;
	static const utility::string_t SCOPES;
	
	static const utility::string_t REDIRECT_URI;

	static const uri BASE_API_URI;

};

