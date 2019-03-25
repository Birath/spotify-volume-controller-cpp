#include "http_utils.h"

using namespace web;

web::json::value get_json_response_body(const web::http::http_response & response) {
	return response.extract_json().get();
	
}

using namespace http;
pplx::task<http_response> request(const web::uri &address, const method http_method) {
	client::http_client client(address);
	return client.request(http_method).then([](http_response response) {
		return response;
	});
}

pplx::task<http_response> request(http_request &request, const web::uri &address) {
	client::http_client client(address);
	return client.request(request).then([](http_response response) {
		return response;
	});
}