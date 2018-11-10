#include "SpotifyListener.h"

using namespace boost::asio;
using namespace web;
using namespace web::http;

SpotifyListener::SpotifyListener(const uri &address) {
	m_server = http_listener(address);
	m_server.support(std::bind(&SpotifyListener::default_handler, this, std::placeholders::_1));
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

void SpotifyListener::default_handler(http_request &request) {
	auto queries = request.request_uri().split_query(request.relative_uri().query());
	if (queries.find(L"code") != queries.end()) {
		m_authorization_code << queries[L"code"];
	}
	else {
		m_authorization_code << queries[L"error"];
	}
	
}

void SpotifyListener::get_authorization_code(std::wstring &authorization_code) {
	while (m_authorization_code.str() == L"") {
		Sleep(10);
	}
	authorization_code = m_authorization_code.str();
}