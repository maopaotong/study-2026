
#define BGFX_CONFIG_DEBUG 1
#include "mapgen.h"
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
#undef BGFX_EMBEDDED_SHADER
#define BGFX_EMBEDDED_SHADER(_name)                                                                                                                  \
    {                                                                                                                                                \
        #_name,                                                                                                                                      \
        {                                                                                                                                            \
            BGFX_EMBEDDED_SHADER_ESSL(bgfx::RendererType::OpenGLES, _name)                                                                           \
            BGFX_EMBEDDED_SHADER_GLSL(bgfx::RendererType::OpenGL, _name){bgfx::RendererType::Noop, (const uint8_t *)"VSH\x5\x0\x0\x0\x0\x0\x0", 10}, \
                {bgfx::RendererType::Count, NULL, 0}                                                                                                 \
        }                                                                                                                                            \
    }

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

    struct MyCallback : public bgfx::CallbackI
    {
        virtual void fatal(
            const char *_filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char *_str)
        {
            std::cout << "BGFX Fatal Error: " << _str << " in " << _filePath << " at line " << _line << std::endl;
        }

        virtual void traceVargs(
            const char *_filePath, uint16_t _line, const char *_format, va_list _argList)
        {
            // 先试一个固定大小缓冲区（大多数日志不会太长）
            char temp[1024];
            int result = vsprintf_s(temp, sizeof(temp), _format, _argList);

            if (result >= 0)
            {
                // 成功
                printf("BGFX [%s:%d] %s", _filePath, _line, temp);
            }
            else
            {
                // 缓冲区太小，动态分配（较少见）
                int len = _vscprintf(_format, _argList);
                if (len > 0)
                {
                    std::vector<char> buffer(len + 1);
                    vsprintf_s(buffer.data(), buffer.size(), _format, _argList);
                    printf("BGFX [%s:%d] %s", _filePath, _line, buffer.data());
                }
            }
            fflush(stdout);
        }

        virtual void profilerBegin(
            const char *_name, uint32_t _abgr, const char *_filePath, uint16_t _line)
        {
            std::cout << "BGFX Profiler Begin: " << _name << std::endl;
        }

        virtual void profilerBeginLiteral(
            const char *_name, uint32_t _abgr, const char *_filePath, uint16_t _line)
        {
            std::cout << "BGFX Profiler Begin Literal: " << _name << std::endl;
        }

        virtual void profilerEnd()
        {
            std::cout << "BGFX Profiler End" << std::endl;
        }

        virtual uint32_t cacheReadSize(uint64_t _id)
        {
            return 0;
        }

        virtual bool cacheRead(uint64_t _id, void *_data, uint32_t _size)
        {
            return false;
        }

        virtual void cacheWrite(uint64_t _id, const void *_data, uint32_t _size)
        {
            // No-op implementation
        }

        virtual void screenShot(
            const char *_filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void *_data, uint32_t _size, bool _yflip)
        {
            std::cout << "BGFX Screenshot saved to: " << _filePath << std::endl;
        }

        virtual void captureBegin(
            uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip)
        {
            std::cout << "BGFX Capture Begin: " << _width << "x" << _height << std::endl;
        }

        virtual void captureEnd()
        {
            std::cout << "BGFX Capture End" << std::endl;
        }

        virtual void captureFrame(const void *_data, uint32_t _size)
        {
            // No-op implementation
        }
    };

    // 初始化时注册

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

    // static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    //     BGFX_EMBEDDED_SHADER(s00_vertex),
    //     BGFX_EMBEDDED_SHADER(s00_fragment),
    //     BGFX_EMBEDDED_SHADER_END(),
    // };

    // static bgfx::ProgramHandle loadEmbeddedProgram(const char *vertName, const char *fragName)
    // {
    //     bgfx::RendererType::Enum type = bgfx::getRendererType();

    //     bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, vertName);
    //     bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, fragName);
    //     bgfx::ProgramHandle handle = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

    //     return handle;
    // }

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
        bgfx::ProgramHandle m_program = loadProgram("s00", "s00", "glsl");
        if (!bgfx::isValid(m_program))
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

            const bgfx::Stats *stats = bgfx::getStats();
            bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);

            bgfx::submit(v0, m_program);
            // bgfx::touch(v0);
            bgfx::frame();
            glfwWaitEventsTimeout(0.01); // 16ms ≈ 60Hz
            counter++;
        }
        bgfx::destroy(m_program);
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