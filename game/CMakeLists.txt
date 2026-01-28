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
find_package(imgui CONFIG REQUIRED)

# shaderc
set(SHADERC "D:/git/bgfx.cmake/build/cmake/bgfx/Debug/shaderc.exe")
set(SHADER_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(GEN_DIR  ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(VARYING  ${SHADER_DIR}/varying.def.sc)
set(BGFX_SHADER_INCLUDE "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/bgfx")
file(MAKE_DIRECTORY ${GEN_DIR}/shaders/glsl)
foreach(SHADER_TYPE fragment vertex)
foreach(RENDER_TYPE glsl essl)
foreach(P_NAME s00)
        set(SHADER_IN ${SHADER_DIR}/${P_NAME}_${SHADER_TYPE}.sc)
        set(SHADER_OUT ${GEN_DIR}/shaders/${RENDER_TYPE}/${P_NAME}_${SHADER_TYPE}.h)
        if(${RENDER_TYPE} STREQUAL "glsl") 
            set(PROFILE 130)
        elseif(${RENDER_TYPE} STREQUAL "essl")
            set(PROFILE 320_es)
        endif()
            
        add_custom_command(
            OUTPUT ${SHADER_OUT}
            COMMAND ${SHADERC}
            -f  ${SHADER_IN}
            -o  ${SHADER_OUT}
            --type ${SHADER_TYPE}
            --profile ${PROFILE}
            --platform windows
            -i ${SHADER_DIR}
            -i ${BGFX_SHADER_INCLUDE}
            --varyingdef ${VARYING}
            --bin2c ${P_NAME}_${SHADER_TYPE}_${RENDER_TYPE}
            DEPENDS ${SHADER_IN} ${VARYING}    
            )        
        list(APPEND SHADER_OUTPUTS ${SHADER_OUT})            
endforeach()
endforeach()
endforeach()
add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})

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


target_link_libraries(game PRIVATE fog-util bgfx::bgfx bgfx::bx bgfx::bimg bgfx::bimg_decode glfw imgui::imgui)        
#
target_compile_options(game PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)

target_include_directories(game PRIVATE include 
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

