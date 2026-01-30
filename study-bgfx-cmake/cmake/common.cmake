
function(compile_bgfx_shader P_NAME)
    # shaderc
    set(SHADERC "D:/git/bgfx.cmake/build/cmake/bgfx/Debug/shaderc.exe")
    set(SHADER_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    set(GEN_DIR  ${CMAKE_CURRENT_BINARY_DIR}/generated)
    set(VARYING  ${SHADER_DIR}/varying.def.sc)
    set(BGFX_SHADER_INCLUDE "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/bgfx")
    foreach(RENDER_TYPE glsl essl)
    file(MAKE_DIRECTORY ${GEN_DIR}/shaders/${RENDER_TYPE})
    foreach(SHADER_TYPE fragment vertex)
        set(SHADER_IN ${SHADER_DIR}/${P_NAME}_${SHADER_TYPE}.sc)
        set(SHADER_OUT ${GEN_DIR}/shaders/${RENDER_TYPE}/${P_NAME}_${SHADER_TYPE}.sc.bin)
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
#--bin2c ${P_NAME}_${SHADER_TYPE}_${RENDER_TYPE}
            DEPENDS ${SHADER_IN} ${VARYING}    
            )        
        list(APPEND SHADER_OUTPUTS ${SHADER_OUT})            
    endforeach()
    endforeach()
    add_custom_target(${APP_NAME}-shaders ALL DEPENDS ${SHADER_OUTPUTS})
endfunction()

function(config_example_project APP_NAME)

    cmake_minimum_required(VERSION 3.16)

    # 设置全局标准（可选）
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    #  find_package
    find_package(bgfx CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    
    compile_bgfx_shader(s00)
    # source files
    file(GLOB_RECURSE SOURCES "src/*.cpp")
    if(MSVC)
        add_compile_options(/Zc:__cplusplus)
        add_compile_options(/Zc:preprocessor)
        add_compile_options(/utf-8)
    endif()
    add_definitions(-DBGFX_CONFIG_DEBUG=1)

    add_executable(${APP_NAME} ${SOURCES})

    target_link_libraries(${APP_NAME} PRIVATE fog-util bgfx::bgfx bgfx::bx bgfx::bimg bgfx::bimg_decode glfw imgui::imgui)        
    #
    target_compile_options(${APP_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
    )

    target_include_directories(${APP_NAME} PRIVATE include 
        ${CMAKE_CURRENT_BINARY_DIR}/generated
    )
endfunction()