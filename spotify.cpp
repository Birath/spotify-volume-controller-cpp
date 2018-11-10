#include "spotify.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility;



pplx::task<void> request(const web::uri &address, const method http_method) {
	std::wcout << "Doing request" << std::endl;
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

void open_uri(const uri &uri)  {
	std::stringstream ss;
	//TODO Implement Linux support
	ss << "cmd /c start \"\" \"";
	// Converts wstring to string
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	ss << converter.to_bytes(uri.to_string());
	ss << "\"";

	// Opens the uri with the std browser on windows
	system(ss.str().c_str());
}

void get_authorization_code(const uri &callback_address) {
	SpotifyListener listener(callback_address);
	listener.open();
	web::uri uri = create_authorization_uri();

	request(uri);
	open_uri(uri);
	
	std::wstring authorization_code;
	listener.get_authorization_code(authorization_code);
	
	//http::http_request request = task.get();
	std::wcout << authorization_code << std::endl;
	listener.close();
}

web::uri create_authorization_uri() {
	web::uri_builder authorization_uri(BASE_AUTHORIZATION_URI);
	authorization_uri.append_query(L"client_id", Config::CLIENT_ID, true);
	authorization_uri.append_query(L"redirect_uri", Config::REDIRECT_URI, true);
	authorization_uri.append_query(L"response_type", L"code", true);
	authorization_uri.append_query(L"scope", Config::SCOPES, true);
	std::wcout << authorization_uri.to_string() << std::endl;

	return authorization_uri.to_uri();
}

