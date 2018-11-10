#pragma once
#include "cpprest/oauth2.h"
#include "cpprest/http_client.h"
#include "cpprest//http_listener.h"
#include <cpprest/filestream.h>
#include <string>
#include <locale>
#include <codecvt>

#include "SpotifyListener.h"
#include "Config.h"
using namespace web::http;



pplx::task<void> request(const web::uri &address, const method http_method = methods::GET);
void get_authorization_code(const web::uri &callback_address);

void open_uri(const web::uri &uri) ;

web::uri create_authorization_uri();

const web::uri BASE_AUTHORIZATION_URI(L"https://accounts.spotify.com/authorize");






