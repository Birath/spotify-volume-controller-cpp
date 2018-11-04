#pragma once
#include <cpprest/http_listener.h>
using namespace web::http::experimental::listener;
class Server
{
public:
	Server(web::uri address);
	~Server();
	void open();
	void close();

private:
	http_listener server;
	bool closed = false;

};

