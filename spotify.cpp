#include "spotify.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility;

pplx::task<void> request(web::uri address, method http_method) {
	
	http_client client(address);
	return client.request(http_method).then([](http_response response) {
		std::wostringstream ss;
		ss << L"Server returned status code: " << response.status_code() << L'.' << std::endl;
		std::wcout << ss.str();
		
		auto bodyStream = response.body();

		ss.str(std::wstring());
		ss << L"Content length is: " << response.headers().content_length() << L"bytes." << std::endl;
		std::wcout << ss.str();
	});
}

void start_listener(uri address) {
	http_listener server(address);
	server.open();
}

