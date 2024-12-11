#include <cstddef>
#include <stdexcept>
#include <string>

#include "windows_helpers.h"

#include <stringapiset.h>
#include <winnls.h>
namespace spotify_volume_controller::windows
{

[[nodiscard]] std::string wide_string_to_string(const std::wstring& wide_string)
{
  if (wide_string.empty()) {
    return "";
  }

  const int size_needed = WideCharToMultiByte(
      CP_UTF8, 0, wide_string.data(), static_cast<int>(wide_string.size()), nullptr, 0, nullptr, nullptr);
  if (size_needed <= 0) {
    throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
  }

  std::string result(static_cast<size_t>(size_needed), 0);
  WideCharToMultiByte(CP_UTF8,
                      0,
                      wide_string.data(),
                      static_cast<int>(wide_string.size()),
                      result.data(),
                      size_needed,
                      nullptr,
                      nullptr);
  return result;
}

// https://gist.github.com/rosasurfer/33f0beb4b10ff8a8c53d943116f8a872
[[nodiscard]] std::wstring string_to_wide_string(const std::string &str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring w_str(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &w_str[0], size_needed);
	return w_str;
}


}  // namespace spotify_volume_controller::windows