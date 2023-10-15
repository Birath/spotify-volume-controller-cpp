#include "Client.h"
#include "http_utils.h"
#include "oauth.h"

const uri Client::BASE_API_URI(L"https://api.spotify.com");

Client::Client(json::value &token_info, const Config &config) : m_token_info(token_info), m_config(config) {
	client = new http::client::http_client(BASE_API_URI);
}

Client::~Client () {
	free(client);
}

http::http_response Client::api_request(const utility::string_t & endpoint, const http::method http_method) {
	http::http_request request;
	request.set_request_uri(endpoint);
	request.set_method(http_method);
	authorize_header(request);

	return client->request(request).get();
}

http::http_response Client::api_request(const utility::string_t & endpoint, const http::method http_method, const utility::string_t &query) {
	uri_builder api_uri(BASE_API_URI);
	api_uri.append_path(endpoint);
	api_uri.append_query(query, true);
	
	return api_request(api_uri.to_string(), http_method);
}

json::value Client::get_device_info() {
	try {
		http::http_response response = api_request(L"/v1/me/player/devices", http::methods::GET);
		return get_json_response_body(response);
	}
	catch (http::http_exception e) {
		utility::stringstream_t error;
		std::wcout << "Spotify API error:" << std::endl;
		std::string msg = e.error_code().message();

		std::cout << msg.length();
		
		return json::value::null();
	}
}

bool Client::set_device_volume(int volume, const utility::string_t &device_id) {
	if (0 <= volume && volume <= 100) {
		utility::stringstream_t query;
		query << "volume_percent=" << volume;
		if (!device_id.empty()) {
			query << "&";
			query << "device_id=" << device_id;
		}

		http::http_response response = api_request(L"/v1/me/player/volume", http::methods::PUT, query.str());
		if (response.status_code() == http::status_codes::NoContent) {
			return true;
		}
		else if (response.status_code() == http::status_codes::NotFound || response.status_code() == http::status_codes::Forbidden) {
			print_play_error_message(response);
			return false;
		}
		else {
			print_error_message(response);
			return false;
		}
	}
	return false;
	
}

bool Client::set_volume(int volume) {
	return set_device_volume(volume);
}

int Client::get_current_playing_volume() {
	using namespace json;
	value device_info = get_device_info();
	if (device_info.is_null()) {
		return -1;
	}
	array devices = device_info[L"devices"].as_array();

	for (auto&& device: devices) {
		if (device[L"is_active"].as_bool()) {
			// Volume percentage may be null according to documentation
			if (device[L"volume_percent"].is_integer()) return device[L"volume_percent"].as_integer();
		}
	}
	return -1;
}

json::value Client::get_desktop_player() {
	json::array devices = get_device_info()[L"devices"].as_array();

	for (auto&& device: devices) {
		if (device[L"type"].as_string().compare(L"Computer") == 0) return device;
	}
	return json::value::Null;
}

void Client::authorize_header(http::http_request request) {
	if (token_is_expired(m_token_info)) {
		refresh_token(m_token_info, m_config);
	}
	request.headers()[L"Authorization"] = L"Bearer " + m_token_info[ACCESS_TOKEN].as_string();
}

void Client::print_error_message(const http::http_response &response) {
	utility::stringstream_t error_stream;
	json::value body = get_json_response_body(response);
	json::value error_body = body[L"error"];
	error_stream << "Status Code: " << error_body[L"status"] << '\n';
	error_stream << "Error message: " << error_body[L"message"] << '\n';
	std::wcout << error_stream.str();
	std::wcout << std::endl;
}

void Client::print_play_error_message(const http::http_response &response) {
	utility::stringstream_t error_stream;
	json::value body = get_json_response_body(response);
	json::value error_body = body[L"error"];
	error_stream << "Status code: " << error_body[L"status"] << '\n';
	error_stream << "Error message: " << error_body[L"message"] << '\n';
	error_stream << "Reason: " << error_body[L"reason"];
	std::wcout << error_stream.str();
	std::wcout << std::endl;
}




