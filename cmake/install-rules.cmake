install(
    TARGETS spotify_volume_controller_exe
    RUNTIME COMPONENT spotify_volume_controller_Runtime
    RESOURCE DESTINATION bin
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
