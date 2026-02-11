
#define BGFX_CONFIG_DEBUG 1
#include <iostream>
#include <bgfx/bgfx.h>


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
            // 
            char temp[1024];
            int result = vsprintf_s(temp, sizeof(temp), _format, _argList);

            if (result >= 0)
            {
                // 
                printf("BGFX [%s:%d] %s", _filePath, _line, temp);
            }
            else
            {
                // 
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

}