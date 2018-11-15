#include "Client.h"
#include "HttpUtils.h"

Client::Client(json::value &token_info) : m_token_info(token_info), BASE_API_URI(L"https://api.spotify.com") {};


Client::~Client() {
}

pplx::task<http::http_response> Client::api_request(const utility::string_t & endpoint, const http::method http_method) {
	http::http_request request;
	request.set_request_uri(endpoint);
	request.set_method(http_method);
	authorize_header(request);

	uri_builder api_uri(BASE_API_URI);
	http::client::http_client client(api_uri.to_uri());

	return client.request(request).then([](http::http_response response) {
		return response;
	});
}

pplx::task<http::http_response> Client::api_request(const utility::string_t & endpoint, const json::value data, const http::method http_method) {
	uri_builder api_uri(BASE_API_URI);
	api_uri.append_path(endpoint);
	api_uri.append_query(data.serialize());
	std::wcout << data.serialize() << std::endl;
	//utility::conversions::print_string(api_uri.to_string());
	return api_request(api_uri.to_string(), http_method);
}

pplx::task<json::value> Client::get_device_info() {
	return api_request(L"/v1/me/player/devices", http::methods::GET).then([](http::http_response response) {
		std::wcout << get_json_response_body(response) << std::endl;
		return response.extract_json();
	});
}

void Client::authorize_header(http::http_request request) {
	//request.headers().add(L"Authorization", L"Bearer " + m_token_info[ACCESS_TOKEN].serialize());
	request.headers()[L"Authorization"] = L"Bearer " + m_token_info[ACCESS_TOKEN].as_string();
}




