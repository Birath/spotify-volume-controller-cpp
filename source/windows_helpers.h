#pragma once
#include <string>

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace spotify_volume_controller::windows
{

[[nodiscard]] std::string wide_string_to_string(const std::wstring& wide_string);

}