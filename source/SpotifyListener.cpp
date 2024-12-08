#include <chrono>
#include <thread>

#include "SpotifyListener.h"

#include <cpprest/base_uri.h>
#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

using namespace web;
using namespace web::http;

namespace spotify_volume_controller
{

SpotifyListener::SpotifyListener(const uri& address)
{
  m_server = experimental::listener::http_listener(address);
  m_server.support([&](http_request request) { request_handler(request); });
}
SpotifyListener::SpotifyListener() = default;

SpotifyListener::~SpotifyListener()
{
  if (!closed) {
    close();
  }
}

void SpotifyListener::open()
{
  // TODO Maybe change to local reference. See https://stackoverflow.com/a/42029220
  m_server.open().wait();
  closed = false;
}

void SpotifyListener::close()
{
  m_server.close().wait();
  closed = true;
}

void SpotifyListener::request_handler(http_request& request)
{
  auto queries = uri::split_query(request.relative_uri().query());
  if (queries.find(L"code") != queries.end()) {
    m_authorization_code << queries[L"code"];
  } else {
    m_authorization_code << L"Error: " << queries[L"error"];
  }
}

auto SpotifyListener::get_authorization_code() -> utility::string_t
{
  while (m_authorization_code.str().empty()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return m_authorization_code.str();
}
}  // namespace spotify_volume_controller