#pragma once
#include <cpprest/http_listener.h>
#include <boost/asio/ssl.hpp>

using namespace web::http::experimental::listener;
using namespace boost::asio;
class Server
{
public:
	Server(web::uri address);
	~Server();
	void open();
	void close();
private:

	void default_handler(web::http::http_request request);

	http_listener m_server;
	bool closed = false;

};

