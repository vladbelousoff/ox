cmake_minimum_required(VERSION 4.0)
project(ox C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(RAYLIB_VERSION 5.5)
set(RAYLIB_NUKLEAR_VERSION 5.5.1)
set(BITSET_VERSION 0.2.2)

file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/code/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/code/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

include(FetchContent)

FetchContent_Declare(
        raylib
        URL "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz"
        FIND_PACKAGE_ARGS ${RAYLIB_VERSION}
)

FetchContent_Declare(
        bitset
        URL "https://github.com/lemire/cbitset/archive/refs/tags/v${BITSET_VERSION}.tar.gz"
        FIND_PACKAGE_ARGS ${BITSET_VERSION}
)

FetchContent_Declare(
        raylib_nuklear
        URL "https://github.com/RobLoach/raylib-nuklear/archive/refs/tags/v${RAYLIB_NUKLEAR_VERSION}.tar.gz"
        FIND_PACKAGE_ARGS ${RAYLIB_NUKLEAR_VERSION}
)

FetchContent_MakeAvailable(
        raylib
        raylib_nuklear
        bitset
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OX_DEBUG_BUILD>
        $<$<CONFIG:RelWithDebInfo>:OX_DEBUG_BUILD>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
        RAYLIB_NUKLEAR_IMPLEMENTATION
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        raylib_nuklear
        cbitset
)
