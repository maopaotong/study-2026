#include "mapgen.h"
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
#define BGFX_EMBEDDED_SHADER(_name)                                                        \
	{                                                                                      \
		#_name,                                                                            \
		{                                                                                  \
			BGFX_EMBEDDED_SHADER_ESSL (bgfx::RendererType::OpenGLES,   _name)              \
			BGFX_EMBEDDED_SHADER_GLSL (bgfx::RendererType::OpenGL,     _name)              \
			{ bgfx::RendererType::Noop,  (const uint8_t*)"VSH\x5\x0\x0\x0\x0\x0\x0", 10 }, \
			{ bgfx::RendererType::Count, NULL, 0 }                                         \
		}                                                                                  \
	}

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

#define BGFX_CONFIG_DEBUG 1

namespace mg
{

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
        bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer.
        bgfxInit.platformData.nwh = glfwGetWin32Window(window);
        bgfxInit.resolution.width = WNDW_WIDTH;
        bgfxInit.resolution.height = WNDW_HEIGHT;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;

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
            bgfx::submit(v0, m_program);
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