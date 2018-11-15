#include "spotify.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility;



pplx::task<void> request(const web::uri &address, const method http_method) {
	std::wcout << "Doing request" << std::endl;
	std::wcout << address.to_string() << std::endl;
	http_client client(address);
	return client.request(http_method).then([](http_response response) {
		std::wostringstream ss;
		ss << L"Server returned status code: " << response.status_code() << L'.' << std::endl;
		std::wcout << ss.str();
		
		ss.clear();
		ss << L"Content length is: " << response.headers().content_length() << L"bytes." << std::endl;
		ss << L"Response body: " << "\n";
		ss << get_json_response_body(response) << std::endl;
		std::wcout << ss.str();
	});
}

pplx::task<http_response> request(http_request &request, const web::uri &address) {
	http_client client(address);
	return client.request(request).then([](http_response response) {
		if (response.status_code() != 200) {
			utility::ostringstream_t ss;
			ss << L"Server returned error code: " << response.status_code() << L'.' << std::endl;
			utility::conversions::print_string(ss.str());
			ss.clear();
			ss << L"Response body: " << "\n";
			ss << get_json_response_body(response) << std::endl;
			utility::conversions::print_string(ss.str());
			// TODO check if error handeling is needed
		}
		return response;
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

std::wstring get_authorization_code(const uri &callback_address) {
	SpotifyListener listener(callback_address);
	listener.open();
	web::uri uri = create_authorization_uri();
	//request(uri);
	open_uri(uri);
	
	std::wstring authorization_code;
	listener.get_authorization_code(authorization_code);
	listener.close();
	return authorization_code;
}

web::json::value get_token(const std::wstring &authorization_code) {
	http::http_request token_request = create_token_request(authorization_code);

	pplx::task<http_response> response_task = request(token_request, Config::BASE_AUTHENTICATION_API_URI);
	response_task.wait();
	http_response response = response_task.get();
	return get_json_response_body(response);
}

http::http_request create_token_request(const utility::string_t &authorization_code)  {
	http_request token_request;
	token_request.set_request_uri(BASE_TOKEN_URI);
	std::wstring body = L"grant_type=authorization_code&";
	body.append(L"code=" + authorization_code);
	body.append(L"&redirect_uri=" + Config::REDIRECT_URI);

	token_request.set_body(body);

	token_request.headers().set_content_type(L"application/x-www-form-urlencoded");
	token_request.headers().add(L"Authorization", L"Basic " + get_authorize_string());
	token_request.set_method(methods::POST);
	return token_request;
}

utility::string_t get_authorize_string() {
	utility::string_t unencoded;
	unencoded.append(Config::CLIENT_ID + L":" + Config::CLIENT_SECRET);
	std::vector<unsigned char> byte_vector(unencoded.begin(), unencoded.end());
	return utility::conversions::to_base64(byte_vector);
}

web::uri create_authorization_uri() {
	web::uri_builder authorization_uri(BASE_AUTHORIZATION_URI);
	authorization_uri.append_query(L"client_id", Config::CLIENT_ID, true);
	authorization_uri.append_query(L"redirect_uri", Config::REDIRECT_URI, true);
	authorization_uri.append_query(L"response_type", L"code", true);
	authorization_uri.append_query(L"scope", Config::SCOPES, true);
	return authorization_uri.to_uri();
}


