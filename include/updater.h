#pragma once
#include <cpprest/uri.h>

web::uri get_latest_uri();

void download_release(web::uri &release_uri);