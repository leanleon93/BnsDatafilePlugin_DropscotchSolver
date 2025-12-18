cmake_minimum_required (VERSION 3.10)

if(NOT WIN32)
    message(FATAL_ERROR "This project only supports Windows.")
endif()

# SET YOUR PLUGIN NAME AND VERSION HERE
set(PLUGIN_NAME "Template")
project ("BnsDatafilePlugin" VERSION 1.0.0)
option(ENABLE_PERSISTENCE "Enable persistence for the plugin configuration" OFF)
option(BNSKR, "Build for BNSKR version" OFF)

# Output build variables for debugging
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ENABLE_PERSISTENCE: ${ENABLE_PERSISTENCE}")
message(STATUS "BNSKR: ${BNSKR}")

if(ENABLE_PERSISTENCE)
    add_compile_definitions(PLUGIN_ENABLE_PERSISTENCE)
else()
    add_compile_definitions(PLUGIN_DISABLE_PERSISTENCE)
endif()
if(BNSKR)
    add_compile_definitions(BNSKR)
endif()
# Ensure Git is available
find_package(Git REQUIRED)

# Always fetch the latest submodules
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(STATUS "Fetching the latest submodules")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
        OUTPUT_VARIABLE GIT_SUBMOD_OUTPUT
        ERROR_VARIABLE GIT_SUBMOD_ERROR
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(STATUS "Git Submodule Output: ${GIT_SUBMOD_OUTPUT}")
        message(STATUS "Git Submodule Error: ${GIT_SUBMOD_ERROR}")
        message(FATAL_ERROR "Failed to fetch the latest submodules. Error code: ${GIT_SUBMOD_RESULT}")
    endif()
endif()

# Verify submodules are present
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/BnsPluginTables/Generated/include/DrEl.h")
    message(FATAL_ERROR "The BnsPluginTables submodule was not downloaded or is missing required files!")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/PluginSdk/BnsPlugin/DrEl.h")
    message(FATAL_ERROR "The PluginSdk submodule was not downloaded or is missing required files!")
endif()

# Include sub-projects.
add_subdirectory ("BnsDatafilePlugin")

