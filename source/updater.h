#pragma once
#include <cpprest/uri.h>
namespace spotify_volume_controller::updater
{

web::uri get_latest_uri();
void download_release(web::uri& release_uri);

}  // namespace spotify_volume_controller::updater