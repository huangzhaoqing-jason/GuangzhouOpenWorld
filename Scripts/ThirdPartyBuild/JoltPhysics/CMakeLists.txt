cmake_minimum_required(VERSION 3.25)
project(JoltPhysics VERSION 6.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -ffast-math -DNDEBUG -DJPH_NO_DEBUG -DJPH_DISABLE_CUSTOM_ALLOCATOR -DJPH_ENABLE_ASSERTS=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=apple-m1 -fvectorize -funroll-loops")

set(JOLT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins/JoltPhysics/Source/ThirdParty/Jolt")

file(GLOB_RECURSE JOLT_SOURCES
    "${JOLT_ROOT}/Jolt/*.cpp"
)

add_library(Jolt STATIC ${JOLT_SOURCES})

target_include_directories(Jolt PUBLIC
    "${JOLT_ROOT}"
)

target_compile_definitions(Jolt PUBLIC
    JPH_PLATFORM_MACOS=1
    JPH_USE_NEON=1
    JPH_CPU_ADDRESS_BITS=64
    JPH_OBJECT_LAYER_BITS=16
)

set_target_properties(Jolt PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Plugins/JoltPhysics/Lib/Mac"
    POSITION_INDEPENDENT_CODE ON
)

message(STATUS "Jolt Physics v6.0.1 build configured for Apple Silicon arm64")
