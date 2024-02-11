#include <cpprest/uri.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/containerstream.h>
#include <cpprest/filestream.h>
#include <cpprest/producerconsumerstream.h>

#include "updater.h"
#include "http_utils.h"

const web::uri RELEASE_URI(L"https://api.github.com/repos/Birath/spotify-volume-controller-cpp/releases");

web::uri get_latest_uri() {
	using namespace web::http;
	http_response response = request(RELEASE_URI).get();
	if (response.status_code() == status_codes::OK) {
		web::json::array releases = get_json_response_body(response).as_array();
		if (releases[0][L"assets"].is_array())
			for (auto&& asset: releases[0][L"assets"].as_array()) {
				if (asset[L"content_type"].as_string().compare(L"application/x-msdownload") == 0) {
					return web::uri(asset[L"browser_download_url"].as_string());
				}
			}
		else {
			std::wcout << L"not array" << std::endl;
		}
	}
	return web::uri();
}

void download_release(web::uri &release_uri) {
	using namespace web::http;

	auto stream = Concurrency::streams::fstream::open_ostream(L"spotify-volume-controller-cpp-tmp.exe", std::ios::in | std::ios::binary).then([=](pplx::streams::basic_ostream<uint8_t> buf) {
		request(release_uri).get().body().read_to_end(buf.streambuf()).get();
		buf.close();
	});
	
}



