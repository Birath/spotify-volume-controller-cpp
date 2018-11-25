#include <locale>
#include <codecvt>
#include <chrono>

#include "oauth.h"
#include "http_utils.h"
#include "Config.h"
#include "SpotifyListener.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility;

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

utility::string_t get_authorization_code(const uri &callback_address) {
	SpotifyListener listener(callback_address);
	listener.open();

	web::uri uri = create_authorization_uri();
	open_uri(uri);
	
	utility::string_t authorization_code = listener.get_authorization_code();
	listener.close();
	return authorization_code;
}

web::json::value get_token(const utility::string_t &authorization_code) {
	http::http_request token_request = create_token_request(authorization_code, L"authorization_code");

	pplx::task<http_response> response_task = request(token_request, Config::BASE_AUTHENTICATION_API_URI);
	response_task.wait();
	http_response response = response_task.get();
	web::json::value token = get_json_response_body(response).get();

	// Adds the epoch time when the token will expire
	auto expiration_time = std::chrono::system_clock::now().time_since_epoch();
	expiration_time += std::chrono::seconds(token[L"expires_in"].as_integer());
	token[L"expires_at"] = expiration_time.count();
	save_token(token);
	return token;
}

web::json::value get_token() {
	web::json::value token = read_token();
	if (token.is_null()) {
		return get_token(get_authorization_code(Config::REDIRECT_URI));
	}
	else if (token_is_expired(token)) {
		refresh_token(token);
	}
	return token;
}

void refresh_token(web::json::value & token) {
	auto token_request = create_token_request(token.at(L"refresh_token").as_string(), L"refresh_token");
	pplx::task<http_response> response_task = request(token_request, Config::BASE_AUTHENTICATION_API_URI);
	response_task.wait();
	http_response response = response_task.get();

	web::json::value new_token = get_json_response_body(response).get();
	token[L"access_token"] = new_token.at(L"access_token");
	token[L"token_type"] = new_token.at(L"token_type");
	token[L"scope"] = new_token.at(L"scope");
	token[L"expires_in"] = new_token.at(L"expires_in");

	// Adds the epoch time when the token will expire
	auto expiration_time = std::chrono::system_clock::now().time_since_epoch();
	expiration_time += std::chrono::seconds(token[L"expires_in"].as_integer());
	token[L"expires_at"] = expiration_time.count();
	save_token(token);
}

void save_token(const web::json::value & token) {
	std::ofstream token_file(TOKEN_FILE_NAME);
	if (token_file) {
		token.serialize(token_file);
		token_file.close();
	}
	else {
		std::wcout << "Failed to write token to file" << "\n";
	}
}

web::json::value read_token() {
	utility::ifstream_t token_file(TOKEN_FILE_NAME);
	
	if (token_file) {
		web::json::value token = json::value::parse(token_file);
		return token;
	}
	else {
		std::wcout << "Failed to read token from file" << "\n";
		return json::value::null();
	}
}

bool token_is_expired(const web::json::value & token) {
	std::wcout << "Checking if token expired" << std::endl;
	using namespace std::chrono;
	auto expiration_epoch_time = token.at(L"expires_at").as_number().to_uint64();
	std::wcout << expiration_epoch_time << std::endl;
	//milliseconds expiration_duration(expiration_epoch_time);
	//std::wcout << expiration_duration.count() << std::endl;
	//time_point<system_clock> expiration_clock(expiration_duration);
	//time_point<system_clock()> expiration_clock(expiration_duration);
	//std::wcout << system_clock::from_time_t(expiration_duration).time_since_epoch().count() << std::endl;
	if (system_clock::now().time_since_epoch().count() > expiration_epoch_time) {
		std::wcout << "Token expired" << std::endl;
		return true;
	}
	return false;
}


http::http_request create_token_request(const utility::string_t &authorization_code, const utility::string_t &grant_type)  {
	http_request token_request;
	token_request.set_request_uri(BASE_TOKEN_URI);
	utility::string_t body = L"grant_type=" + grant_type;
	if (grant_type == L"authorization_code") {
		body.append(L"&code=" + authorization_code);
		body.append(L"&redirect_uri=" + Config::REDIRECT_URI);
	} 
	else {
		body.append(L"&refresh_token=" + authorization_code);
	}

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
