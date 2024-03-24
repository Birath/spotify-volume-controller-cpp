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
}  // namespace spotify_volume_controller::windows