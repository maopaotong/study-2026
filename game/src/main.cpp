
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
#include "shaders/glsl/s00_vertex.h"
#include "shaders/glsl/s00_fragment.h"
#include "shaders/essl/s00_vertex.h"
#include "shaders/essl/s00_fragment.h"
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

    static bgfx::ProgramHandle loadProgram(std::string vertFile, std::string fragFile)
    {
        const bgfx::Memory *vMem = FileUtil::load(vertFile);
        const bgfx::Memory *fMem = FileUtil::load(fragFile);
        bgfx::ShaderHandle vsh = bgfx::createShader(vMem);
        bgfx::ShaderHandle fsh = bgfx::createShader(fMem);
        bgfx::ProgramHandle handle = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

        return handle;
    }

    static const bgfx::EmbeddedShader s_embeddedShaders[] = {
        BGFX_EMBEDDED_SHADER(s00_vertex),
        BGFX_EMBEDDED_SHADER(s00_fragment),
        BGFX_EMBEDDED_SHADER_END(),
    };

    static bgfx::ProgramHandle loadEmbeddedProgram(const char *vertName, const char *fragName)
    {
        bgfx::RendererType::Enum type = bgfx::getRendererType();

        bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, vertName);
        bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, fragName);
        bgfx::ProgramHandle handle = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

        return handle;
    }

    int _main(int argc, char **argv)
    {
        glfwSwapInterval(1); // Enable vsync
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow *window = glfwCreateWindow(1024, 768, "Hello.bgfx", nullptr, nullptr);

        bgfx::PlatformData pd;
        pd.nwh = glfwGetWin32Window(window);
        bgfx::setPlatformData(pd);

        bgfx::renderFrame();

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

        bgfx::ProgramHandle m_program = loadEmbeddedProgram("s00_vertex", "s00_fragment");

        // bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
        const bgfx::ViewId v0 = 0;
        // bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
        bgfx::setViewClear(v0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::touch(v0);
        bgfx::setViewRect(v0, 0, 0, bgfx::BackbufferRatio::Equal);
        unsigned int counter = 0;
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            const bgfx::Stats* stats = bgfx::getStats();
            
			bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
				, stats->width
				, stats->height
				, stats->textWidth
				, stats->textHeight
				);

            //bgfx::submit(v0, m_program);
            bgfx::touch(v0);
            bgfx::frame();
            glfwWaitEventsTimeout(0.01); // 16ms ≈ 60Hz
            counter++;
        }

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