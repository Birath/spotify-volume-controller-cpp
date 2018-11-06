#pragma once
#include <string>
#include <vector>
class Credentials
{
public:
	Credentials();
	~Credentials();

	static std::string CLIENT_CREDENTIALS;
	static std::string CLIENT_SECRET;
	static std::vector<std::string> SCOPES;
};

