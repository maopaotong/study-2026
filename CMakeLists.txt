#
cmake_minimum_required(VERSION 3.16)
option(BUILD_GAME "Build game" ON)

set(PROJECT_NAME, "game")
project(game LANGUAGES CXX)

# 设置全局标准（可选）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#  find_package
find_package(bgfx CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    add_compile_options(/Zc:preprocessor)
    add_compile_options(/utf-8)
endif()

add_executable(game ${SOURCES})

get_target_property(COMPILE_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)

message(STATUS "C++ Compile Flags: ${COMPILE_FLAGS}")

target_link_libraries(game PRIVATE bgfx::bgfx glfw)        
#
target_compile_options(game PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)

target_include_directories(game PRIVATE include)

