#include "SpotifyListener.h"

using namespace boost::asio;
using namespace web;
using namespace web::http;

SpotifyListener::SpotifyListener(const uri &address) {
	m_server = experimental::listener::http_listener(address);
	m_server.support(std::bind(&SpotifyListener::request_handler, this, std::placeholders::_1));
}
SpotifyListener::SpotifyListener() {
}


SpotifyListener::~SpotifyListener() {
	if (!closed) {
		close();
	}
}

void SpotifyListener::open() {
	//TODO Maybe change to local refference. See https://stackoverflow.com/a/42029220
	m_server.open().wait();
	closed = false;
}

void SpotifyListener::close() {
	m_server.close().wait();
	closed = true;
}

void SpotifyListener::request_handler(http_request &request) {
	auto queries = uri::split_query(request.relative_uri().query());
	if (queries.find(L"code") != queries.end()) {
		m_authorization_code << queries[L"code"];
	}
	else {
		m_authorization_code << queries[L"error"];
	}
	
}

utility::string_t SpotifyListener::get_authorization_code() {
	while (m_authorization_code.str() == L"") {
		Sleep(10);
	}
	return m_authorization_code.str();
}