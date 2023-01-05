cmake_minimum_required (VERSION 3.18)
project ("mlnserver")

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

option(SPDLOG_BUILD_WARNINGS "Enable compiler warnings" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

INCLUDE (CheckIncludeFiles)
include(ExternalProject)
include(fmt)
include(spdlog)
include(mlnsdk)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)

    ADD_DEFINITIONS(/bigobj)
    add_definitions(-D_WIN32_WINNT=0x0601)
    ADD_DEFINITIONS(-DCMAKE_TOOLCHAIN_FILE=C:/local/vcpkg/scripts/buildsystems/vcpkg.cmake)
    set(CMAKE_CXX_FLAGS                "/EHs /EHsc- /GL- /DWIN32 /D_WINDOWS /GR")
endif(MSVC)

include_directories(
    ${FMT_INCLUDE_DIR} 
    ${SPDLOG_INCLUDE_DIR} 
    ${MLNSDK_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/mlnserver/
    ${CMAKE_SOURCE_DIR}/net/
)


add_subdirectory("mlnserver")
add_dependencies(mlnserver spdlog mlnsdk)