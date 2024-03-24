# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(spotify_volume_controllerTests LANGUAGES CXX)

# ---- Dependencies ----

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(spotify_volume_controller_test source/spotify_volume_controller_test.cpp)
target_link_libraries(
    spotify_volume_controller_test PRIVATE
    Catch2::Catch2WithMain
)
target_compile_features(spotify_volume_controller_test PRIVATE cxx_std_20)

catch_discover_tests(spotify_volume_controller_test)

# ---- End-of-file commands ----

add_folders(Test)
