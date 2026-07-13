cmake_minimum_required(VERSION 3.25)
project(SoLoud VERSION 2026 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -ffast-math -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=apple-m1 -fvectorize -funroll-loops")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto -DNDEBUG")

set(SOLOUD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins/SoLoud/Source/ThirdParty/SoLoud")

file(GLOB_RECURSE SOLOUD_SOURCES
    "${SOLOUD_ROOT}/src/backend/coreaudio/*.cpp"
    "${SOLOUD_ROOT}/src/core/*.cpp"
    "${SOLOUD_ROOT}/src/audiosource/wav/*.cpp"
    "${SOLOUD_ROOT}/src/audiosource/ogg/*.cpp"
    "${SOLOUD_ROOT}/src/filter/*.cpp"
)

add_library(SoLoud STATIC ${SOLOUD_SOURCES})

target_include_directories(SoLoud PUBLIC
    "${SOLOUD_ROOT}/include"
)

target_compile_definitions(SoLoud PUBLIC
    SOLOUD_PLATFORM_MACOS=1
    SOLOUD_COREAUDIO=1
    SOLOUD_ARM64_NEON=1
    WITH_SOLOUD=1
)

set_target_properties(SoLoud PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins/SoLoud/Lib/Mac"
    POSITION_INDEPENDENT_CODE ON
)

message(STATUS "SoLoud 2026 build configured for Apple Silicon arm64 / Core Audio")
