include(FetchContent)
FetchContent_Declare(
    uiohook
    GIT_REPOSITORY https://github.com/kwhat/libuiohook
    GIT_TAG 23acecfe207f8a8b5161bec97a8a6fd6ad0aea88 # 1.2.2
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(uiohook)