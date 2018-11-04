#include "Server.h"



Server::Server(web::uri address) {
	server = http_listener(address);
}


Server::~Server() {
	if (closed) {
		server.~http_listener();
	}
	else {
		close();
		server.~http_listener();

	}

}

void Server::open() {
	//TODO Maybe change to local refference. See https://stackoverflow.com/a/42029220
	server.open().then([this](){
			closed = false;
		});
	

}

void Server::close() {
	server.close().then([this] {
			closed = true;
		});
	
}
