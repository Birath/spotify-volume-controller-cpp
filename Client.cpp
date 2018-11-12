#include "Client.h"

Client::Client(json::value &token_info) : m_token_info(token_info) {};


Client::~Client()
{
}

pplx::task<http::http_response> Client::api_request(const utility::string_t & endpoint, const http::method http_method) {
	uri_builder api_uri(Config::BASE_API_URI);
	api_uri.append_path(endpoint);
	http::client::http_client client(api_uri.to_uri());
	return client.request(http_method).then([](http::http_response response) {
		return response;
	});
}

pplx::task<http::http_response> Client::api_request(const utility::string_t & endpoint, const json::value data, const http::method http_method) {
	uri_builder api_uri(Config::BASE_API_URI);
	api_uri.append_path(endpoint);
	api_uri.append_query(data.serialize());
	std::wcout << data.serialize() << std::endl;
	//utility::conversions::print_string(api_uri.to_string());
	return api_request(api_uri.to_string(), http_method);
}




