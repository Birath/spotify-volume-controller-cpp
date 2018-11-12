#include "HttpUtils.h"

json::value get_json_response_body(const http::http_response & response) {
	pplx::task<web::json::value> body_task = response.extract_json();
	body_task.wait();
	web::json::value response_body = body_task.get();
	return response_body;
}
