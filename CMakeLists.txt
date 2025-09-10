cmake_minimum_required(VERSION 3.16)
project(ox C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(RAYLIB_VERSION 5.5)
set(BITSET_VERSION 0.2.2)

file(GLOB_RECURSE ENGINE_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/code/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/code/*.h"
)

file(GLOB_RECURSE EDITOR_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.h"
)

# Create separate source lists for engine and editor
set(ENGINE_SOURCES ${ENGINE_SOURCE_FILES})
set(EDITOR_SOURCES ${ENGINE_SOURCE_FILES} ${EDITOR_SOURCE_FILES})

# Remove the original entrypoint from editor build
list(REMOVE_ITEM EDITOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/code/ox_entrypoint.c")

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES})
add_executable(ox_editor ${EDITOR_SOURCES})

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

FetchContent_MakeAvailable(
        raylib
        bitset
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OX_DEBUG_BUILD>
        $<$<CONFIG:RelWithDebInfo>:OX_DEBUG_BUILD>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        cbitset
)

target_link_libraries(ox_editor PRIVATE
        raylib
        cbitset
        ${GTK3_LIBRARIES}
)

target_include_directories(ox_editor PRIVATE
        ${GTK3_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/code
)

target_compile_options(ox_editor PRIVATE
        ${GTK3_CFLAGS_OTHER}
)
