
#define BGFX_CONFIG_DEBUG 1
#include <iostream>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <bimg/bimg.h>
#include <string>

#if BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include "FileUtil.h"

#include <bgfx/embedded_shader.h>
#include <stdint.h>
#include <bx/math.h>
#include "MyCallback.h"
#include "ColorMap.h"
#include "DualMap.h"
#include "DualMesh.h"
#include "ShaderUtil.h"
#include "LogUtil.h"
#include "Entity00.h"
#include "Entity01.h"

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

namespace mg
{    
    static void glfw_errorCallback(int error, const char *description)
    {
        // fprintf(stderr, "GLFW error %d: %s\n", error, description);
        std::cout << "GLFW error:" << description << std::endl;
    }
    

    static void setupMesh(DualMap &map)
    {
        DualMesh &mesh = map.mesh;
    }

    int _main(int argc, char **argv)
    {
        std::cout << std::filesystem::current_path() << std::endl;
        glfwSwapInterval(1); // Enable vsync
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow *window = glfwCreateWindow(1024, 768, "Hello.bgfx", nullptr, nullptr);

        bgfx::Init bgfxInit;
        bgfxInit.type = bgfx::RendererType::OpenGL;
        bgfxInit.platformData.nwh = glfwGetWin32Window(window);
        bgfxInit.resolution.width = WNDW_WIDTH;
        bgfxInit.resolution.height = WNDW_HEIGHT;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.debug = true;
        MyCallback callback;
        bgfxInit.callback = &callback;
        bgfx::init(bgfxInit);

        // bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
        const bgfx::ViewId v0 = 0;
        // bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
        bgfx::setViewClear(v0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(v0, 0, 0, bgfx::BackbufferRatio::Equal);

        bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER);
        // bgfx::touch(v0);
        unsigned int counter = 0;
        bgfx::TextureHandle tex = ColorMap::createTexture();
        Entity00 entity00;
        Entity01 entity01;
        if (entity00.init())
        {
            LogUtil::log("Failed to initialize entity00!");
            return -1;
        }
        if (entity01.init())
        {
            LogUtil::log("Failed to initialize entity01!");
            return -1;
        }

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            { // view projection matrix

                const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
                const bx::Vec3 eye = {0.0f, 0.0f, 5.0f};

                float view[16];
                bx::mtxLookAt(view, eye, at);

                float proj[16];
                bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(0, view, proj);
            }

            { // model matrix

                float mtx1[16];
                bx::mtxScale(mtx1, 0.5f);

                float mtx2[16];
                bx::mtxRotateXY(mtx2, counter * 0.01f, counter * 0.01f);

                bx::mtxMul(mtx2, mtx2, mtx1);

                bgfx::setTransform(mtx2);
            }
            { // texture

                bgfx::setTexture(0, bgfx::createUniform("s_colorMap", bgfx::UniformType::Sampler), tex);
            }

            //entity00.submit(0);
            entity01.submit(0);

            bgfx::frame();
            glfwWaitEventsTimeout(0.01); // 16ms ≈ 60Hz
            counter++;
        }
        entity00.destroy();
        entity01.destroy();
        bgfx::shutdown();
        glfwTerminate();
        LogUtil::log("Done.");
        return 0;
    }
};

int main(int argc, char **argv)
{
    return mg::_main(argc, argv);
}