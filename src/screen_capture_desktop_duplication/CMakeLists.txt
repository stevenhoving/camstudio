#project(CamStudioDesktopDupe)

set(SOURCE
	#main.cpp
    #main_old.cpp
	src/dxgi_adapters.cpp
	src/dxgi_device.cpp
	src/dxgi_output_duplication.cpp
	src/dxgi_texture_staging.cpp
	include/screen_capture_desktop_duplication/dxgi_adapters.h
    include/screen_capture_desktop_duplication/dxgi_device.h
	include/screen_capture_desktop_duplication/dxgi_output_duplication.h
    include/screen_capture_desktop_duplication/dxgi_texture_staging.h
)

add_library(screen_capture_desktop_duplication STATIC
    ${SOURCE}
)

source_group(src FILES
	${SOURCE}
)

target_compile_definitions(screen_capture_desktop_duplication
  PUBLIC
    NOMINMAX
)

target_include_directories(screen_capture_desktop_duplication
  PUBLIC
    include
)

target_link_libraries(screen_capture_desktop_duplication
  PUBLIC
    fmt
    dxgi.lib
    d3d11.lib
    #d3dcompiler.lib # we aren't rendering yet
)

#set_target_properties(screen_capture_desktop_duplication PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#)
#
#install(
#    TARGETS screen_capture_desktop_duplication
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib
#)
