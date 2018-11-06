cmake_minimum_required(VERSION 3.12)
project(spotify_volume_controller_cpp)

#set(CMAKE_CXX_STANDARD 17)
find_package(cpprest REQUIRED)
add_executable(spotify_volume_controller_cpp main.cpp)
target_link_libraries(spotify_volume_controller cpprest)