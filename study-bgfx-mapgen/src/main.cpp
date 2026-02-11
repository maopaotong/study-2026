
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

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

namespace mg
{
    struct PosColorVertex
    {
        float x;
        float y;
        float z;
        uint32_t abgr;
    };

    static PosColorVertex vertices[] = {
        {-1.0f, 1.0f, 1.0f, 0xff000000},
        {1.0f, 1.0f, 1.0f, 0xff0000ff},
        {-1.0f, -1.0f, 1.0f, 0xff00ff00},
        {1.0f, -1.0f, 1.0f, 0xff00ffff},
        {-1.0f, 1.0f, -1.0f, 0xffff0000},
        {1.0f, 1.0f, -1.0f, 0xffff00ff},
        {-1.0f, -1.0f, -1.0f, 0xffffff00},
        {1.0f, -1.0f, -1.0f, 0xffffffff},
    };

    static const uint16_t tlist[] = {
        // clang-format off
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,
        0, 2, 4,
        4, 2, 6,
        1, 5, 3,
        5, 7, 3,
        0, 4, 1,
        4, 5, 1,
        2, 3, 6,
        6, 3, 7,
        // clang-format on
    };


    static void glfw_errorCallback(int error, const char *description)
    {
        // fprintf(stderr, "GLFW error %d: %s\n", error, description);
        std::cout << "GLFW error:" << description << std::endl;
    }
    void log(std::string msg)
    {
        std::cout << msg << std::endl;
    }

    static bgfx::ProgramHandle loadProgram(std::string vname, std::string fname, std::string group)
    {
        std::string vfile = "shaders/" + group + "/" + vname + ".vert.sc.bin";
        std::string ffile = "shaders/" + group + "/" + fname + ".frag.sc.bin";
        const bgfx::Memory *vMem = FileUtil::load(vfile);
        const bgfx::Memory *fMem = FileUtil::load(ffile);
        bgfx::ShaderHandle vsh = bgfx::createShader(vMem);
        bgfx::ShaderHandle fsh = bgfx::createShader(fMem);
        bgfx::ProgramHandle handle = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

        return handle;
    }


    int _main(int argc, char **argv)
    {
        std::cout<<std::filesystem::current_path()<<std::endl;
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

        // bgfx::ProgramHandle m_program = loadEmbeddedProgram("s00_vertex", "s00_fragment");
        bgfx::ProgramHandle s00_program = loadProgram("s00", "s00", "glsl");
        if (!bgfx::isValid(s00_program))
        {
            log("Failed to load program!");
            return -1;
        }

        bgfx::ProgramHandle s01_program = loadProgram("s01", "s01", "glsl");
        if (!bgfx::isValid(s01_program))
        {
            log("Failed to load program!");
            return -1;
        }

        // bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
        const bgfx::ViewId v0 = 0;
        // bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
        bgfx::setViewClear(v0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(v0, 0, 0, bgfx::BackbufferRatio::Equal);

        bgfx::VertexLayout vlayout;
        vlayout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

        bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), vlayout);
        bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(tlist, sizeof(tlist)));
        bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER);
        // bgfx::touch(v0);
        unsigned int counter = 0;
        bgfx::TextureHandle tex = ColorMap::createTexture();
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
            {//texture
                
                bgfx::setTexture(0, bgfx::createUniform("s_colorMap", bgfx::UniformType::Sampler), tex);
                

            }
            
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);

            //bgfx::submit(v0, s00_program);
            bgfx::submit(v0, s01_program);
            // bgfx::touch(v0);
            bgfx::frame();
            glfwWaitEventsTimeout(0.01); // 16ms ≈ 60Hz
            counter++;
        }
        bgfx::destroy(s00_program);
        bgfx::destroy(s01_program);
        bgfx::shutdown();
        glfwTerminate();
        log("Done.");
        return 0;
    }
};

int main(int argc, char **argv)
{
    return mg::_main(argc, argv);
}