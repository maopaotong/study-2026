
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

#include "util/FileUtil.h"

#include <bgfx/embedded_shader.h>
#include <stdint.h>
#include <bx/math.h>
#include "MyCallback.h"
#include "mapgen.h"
#include "util.h"
#include "renderable.h"
#include "Entity00.h"
#include "Entity01.h"
#include "Entity02.h"

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900
#define PROJ_FAR 1000000
namespace mg
{
    static void glfw_errorCallback(int error, const char *description)
    {
        // fprintf(stderr, "GLFW error %d: %s\n", error, description);
        std::cout << "GLFW error:" << description << std::endl;
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

        bgfx::setDebug(BGFX_DEBUG_TEXT       //
                       | BGFX_DEBUG_STATS    //
                       | BGFX_DEBUG_PROFILER //
                                             //               | BGFX_DEBUG_WIREFRAME //
        );
        // bgfx::touch(v0);

        ListRenderable list;
        //list.list.push_back(new Entity00());
        list.list.push_back(new Entity01());
        list.list.push_back(new Entity02());

        list.init();

        // const bx::Vec3 eye = {0.0f, 0.0f, -150.0f};//z as camera position
        // bx::Handedness::Enum handedness = bx::Handedness::Right;
        const bx::Vec3 eye = {0.0f, 0.0f, 150.0f}; // z as camera position
        const bx::Vec3 at = {0.0f, 0.0f, 0.0f};    // zero
        const bx::Vec3 up = {0.0f, 1.0f, 0.0f};    // y as up
        float view[16];
        MtxUtil::mtxLookAt(view, eye, at, up);
        
        // printf("View matrix right vector: (%f, %f, %f)\n", view[0], view[4], view[8]);
        printf("Right vector: (%f, %f, %f)\n", view[0], view[1], view[2]);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, PROJ_FAR, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);
        // main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            list.submit(0);

            bgfx::frame();
            glfwWaitEventsTimeout(0.01); // 16ms ≈ 60Hz
        } // end while

        list.destroy();
        // entity01.destroy();
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