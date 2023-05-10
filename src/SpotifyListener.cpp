#include "SpotifyListener.h"
#include <chrono>
#include <thread>


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
	//TODO Maybe change to local reference. See https://stackoverflow.com/a/42029220
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
		m_authorization_code << L"Error: " << queries[L"error"];
	}
	
}

utility::string_t SpotifyListener::get_authorization_code() {
	while (m_authorization_code.str() == L"") {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return m_authorization_code.str();
}