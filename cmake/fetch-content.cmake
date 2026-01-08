include(FetchContent)
FetchContent_Declare(
    uiohook
    GIT_REPOSITORY https://github.com/kwhat/libuiohook
    GIT_TAG 90c2248503bb62f57e7fc1c0ec339053010e209d # 1.3
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(uiohook)