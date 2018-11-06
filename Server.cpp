#include "Server.h"

using namespace boost::asio;
using namespace web;
using namespace web::http;

Server::Server(uri address) {
	m_server = http_listener(address);
	m_server.support(std::bind(&Server::default_handler, this, std::placeholders::_1);

}


Server::~Server() {
	if (closed) {
		m_server.~http_listener();
	}
	else {
		close();
		m_server.~http_listener();

	}

}

void Server::open() {
	//TODO Maybe change to local refference. See https://stackoverflow.com/a/42029220
	m_server.open().then([this](){
			closed = false;
		});
	

}

void Server::close() {
	m_server.close().then([this] {
			closed = true;
		});
}

void Server::default_handler(http_request request) {
	std::cout << request.body() << std::endl;
}