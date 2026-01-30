# game-01/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME, "bgfx-mapgen")
project(Game01 LANGUAGES CXX)

# 设置全局标准（可选）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(fog-util)
add_subdirectory(example-00)
add_subdirectory(example-01)

    

message(STATUS "C++ Compile Flags: ${COMPILE_FLAGS}")


