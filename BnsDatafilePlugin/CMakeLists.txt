configure_file(pluginversion.h.in pluginversion.h @ONLY)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(glaze_ENABLE_AVX2 ON)
endif()

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.dependencies")
include(FetchContent)
FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

add_library(${PROJECT_NAME} SHARED "BnsDatafilePlugin.cpp" "UiPanel.cpp" "plugin_config.cpp")
# Trim whitespace
string(STRIP "${PLUGIN_NAME}" PLUGIN_NAME)

# Replace spaces with underscores
string(REPLACE " " "_" PLUGIN_NAME "${PLUGIN_NAME}")

# Remove special characters
string(REGEX REPLACE "[^a-zA-Z0-9_]" "" PLUGIN_NAME "${PLUGIN_NAME}")

# Convert to lowercase
string(TOLOWER "${PLUGIN_NAME}" PLUGIN_NAME)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/external/BnsPluginTables/Generated/include
    ${CMAKE_SOURCE_DIR}/external/PluginSdk/BnsPlugin
    ${CMAKE_SOURCE_DIR}/external/include
    ${PROJECT_BINARY_DIR}/BnsDatafilePlugin)

target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)

if(MSVC)
    # basic compile options
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor /MP)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /permissive- /sdl)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi> $<$<CONFIG:Release>:/O2>)
    add_compile_options("/utf-8")
    # Match __RuntimeTypeInfo false from the vcxproj (disables RTTI)
    target_compile_options(${PROJECT_NAME} PRIVATE /GR-)

    # IMPORTANT: make Debug use the same CRT as Release/host to avoid ABI mismatch.
    # If host expects /MD (MultiThreadedDLL), use that. If host uses /MDd, change accordingly.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "" FORCE)

    # Disable iterator / secure CRT checks in Debug to match Release ABI (only if you need byte-for-byte ABI parity).
    # This makes Debug STL layout compatible with Release builds of the host.
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>
        $<$<CONFIG:Debug>:_SECURE_SCL=0>
    )
endif()