#include <cstdint>
#include <ios>
#include <iostream>
#include <ostream>

#include "updater.h"

#include <cpprest/base_uri.h>
#include <cpprest/filestream.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>

#include "http_utils.h"

namespace spotify_volume_controller::updater
{

const web::uri release_uri(L"https://api.github.com/repos/Birath/spotify-volume-controller-cpp/releases");

web::uri get_latest_uri()
{
  web::http::http_response const response = request(release_uri).get();
  if (response.status_code() == web::http::status_codes::OK) {
    web::json::array releases = get_json_response_body(response).as_array();
    if (releases[0][L"assets"].is_array()) {
      for (auto&& asset : releases[0][L"assets"].as_array()) {
        if (asset[L"content_type"].as_string() == L"application/x-msdownload") {
          return {asset[L"browser_download_url"].as_string()};
        }
      }
    } else {
      std::wcout << L"not array" << '\n';
    }
  }
  return {};
}

void download_release(web::uri& release_uri)
{
  auto stream = Concurrency::streams::fstream::open_ostream(L"spotify-volume-controller-cpp-tmp.exe",
                                                            std::ios::in | std::ios::binary)
                    .then(
                        [=](const pplx::streams::basic_ostream<uint8_t>& buf)
                        {
                          request(release_uri).get().body().read_to_end(buf.streambuf()).get();
                          buf.close();
                        });
}
}  // namespace spotify_volume_controller::updater