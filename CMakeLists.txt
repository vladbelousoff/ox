cmake_minimum_required(VERSION 3.16)
project(ox C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(RAYLIB_VERSION 5.5)
set(BITSET_VERSION 0.2.2)

# Find GTK3 for the editor
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Engine source files (excluding entrypoint for reusability)
file(GLOB ENGINE_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/code/ox_*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/code/ox_*.h"
)
list(REMOVE_ITEM ENGINE_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/code/ox_entrypoint.c")

# Editor source files
file(GLOB EDITOR_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.h"
)

# Game executable (original)
add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/code/ox_entrypoint.c")

# Editor executable
add_executable(${PROJECT_NAME}_editor ${ENGINE_SOURCE_FILES} ${EDITOR_SOURCE_FILES})

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

# Game executable configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OX_DEBUG_BUILD>
        $<$<CONFIG:RelWithDebInfo>:OX_DEBUG_BUILD>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        cbitset
)

# Editor executable configuration
target_compile_definitions(${PROJECT_NAME}_editor PRIVATE
        $<$<CONFIG:Debug>:OX_DEBUG_BUILD>
        $<$<CONFIG:RelWithDebInfo>:OX_DEBUG_BUILD>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
        OX_EDITOR_BUILD
)

target_include_directories(${PROJECT_NAME}_editor PRIVATE ${GTK3_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME}_editor PRIVATE ${GTK3_CFLAGS_OTHER})
target_link_libraries(${PROJECT_NAME}_editor PRIVATE
        raylib
        cbitset
        ${GTK3_LIBRARIES}
)
