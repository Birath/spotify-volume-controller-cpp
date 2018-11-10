#pragma once
#include <cpprest/http_listener.h>
#include <boost/asio/ssl.hpp>

using namespace web::http::experimental::listener;
using namespace boost::asio;
class SpotifyListener
{
public:
	SpotifyListener(const web::uri &address);
	SpotifyListener();
	~SpotifyListener();

	/// <summary>
	/// Gets the authorization code when it's done or, if it failed, the reason why
	/// </summary>
	/// <returns>The authorization code or error message</returns>
	void get_authorization_code(std::wstring &authorization_code);

	void open();
	void close();
private:

	void default_handler(web::http::http_request &request);

	http_listener m_server;
	std::wstringstream m_authorization_code;
	bool closed = false;

};

