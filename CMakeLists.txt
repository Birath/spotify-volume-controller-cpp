cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    spotify_volume_controller
    VERSION 0.1.7
    DESCRIPTION "A program to hook volume keys directly to Spotify"
    HOMEPAGE_URL "https://github.com/Birath/spotify-volume-controller-cpp"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

find_package(cpprestsdk REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(cpr REQUIRED)

# ---- Declare library ----

add_library(
  spotify_volume_controller_lib OBJECT
  source/Client.cpp
  source/Config.cpp
  source/http_utils.cpp
  source/oauth.cpp
  source/SpotifyListener.cpp
  source/updater.cpp
  source/VolumeController.cpp
  source/windows_helpers.cpp
)

target_include_directories(
    spotify_volume_controller_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(spotify_volume_controller_lib PUBLIC cxx_std_20)
target_compile_definitions(spotify_volume_controller_lib PRIVATE _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1)
target_link_libraries(spotify_volume_controller_lib PRIVATE cpprestsdk::cpprest nlohmann_json::nlohmann_json fmt::fmt cpr::cpr)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

# ---- Declare executable ----

add_executable(spotify_volume_controller_exe 
  source/main.cpp
  resources/config-example.json
)
add_executable(spotify_volume_controller::exe ALIAS spotify_volume_controller_exe)


set_property(TARGET spotify_volume_controller_exe PROPERTY OUTPUT_NAME spotify_volume_controller)
set_property(TARGET spotify_volume_controller_exe PROPERTY RESOURCE resources/config-example.json)

target_compile_features(spotify_volume_controller_exe PRIVATE cxx_std_20)
target_compile_definitions(spotify_volume_controller_exe PRIVATE _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1)
target_compile_definitions(spotify_volume_controller_exe PRIVATE VERSION="${PROJECT_VERSION}")
target_link_libraries(spotify_volume_controller_exe PRIVATE spotify_volume_controller_lib  argparse::argparse cpprestsdk::cpprest)
# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT spotify_volume_controller_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of spotify_volume_controller"
  )
endif()

include(cmake/dev-mode.cmake)
add_custom_command(
  TARGET spotify_volume_controller_exe POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
  ${CMAKE_SOURCE_DIR}/resources/config-example.json
  "$<TARGET_FILE_DIR:spotify_volume_controller_exe>/config-example.json"
)