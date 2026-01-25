#include "mapgen.h"
#include <string>

#if BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

static void glfw_errorCallback(int error, const char *description)
{
    // fprintf(stderr, "GLFW error %d: %s\n", error, description);
    std::cout << "GLFW error:" << description << std::endl;
}
void log(std::string msg)
{
    std::cout << msg << std::endl;
}

int main(int argc, char **argv)
{
    glfwSetErrorCallback(glfw_errorCallback);
    if (!glfwInit())
    {
        log("failed to glfwInit().");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1024, 768, "helloworld", nullptr, nullptr);
    if (!window)
    {
        log("failed to glfwCreateWindow().");
        return 1;
    }

    bgfx::renderFrame();

    bgfx::Init init;
    init.platformData.nwh = glfwGetWin32Window(window);
    init.resolution.reset = BGFX_RESET_VSYNC;


    if (!bgfx::init(init))
    {
        log("failed to init bgfx.");
        return 1;
    }
    while (!glfwWindowShouldClose(window))
    { 
        bgfx::frame();
        glfwWaitEvents();
    }
    bgfx::shutdown();
    glfwTerminate();
    return 0;
}