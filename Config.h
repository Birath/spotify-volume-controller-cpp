#pragma once
#include <string>
#include <vector>
class Config
{
public:
	Config();
	~Config();

	static std::wstring CLIENT_ID;
	static std::wstring CLIENT_SECRET;
	static std::wstring SCOPES;
	
	static std::wstring REDIRECT_URI;
};

