

#include <fstream>
#include <filesystem>
#include <bgfx/bgfx.h>

namespace mg
{
    struct FileUtil
    {
        static const bgfx::Memory* load(std::string file)
        {
            std::filesystem::path fpath(file.c_str());
            if (!std::filesystem::exists(fpath))
            {
                throw std::runtime_error(std::string("no such file:" + file));
            }
            std::ifstream f(fpath);

            if (!f.is_open())
            {
                throw std::runtime_error("failed to open file for read: " + file);
            }
            f.seekg(0, std::ios::end);
            size_t fSize = static_cast<size_t>(f.tellg());
            f.seekg(0, std::ios::beg);
            const bgfx::Memory *mem = bgfx::alloc(fSize);
            f.read(reinterpret_cast<char*>(mem->data), fSize);
            f.close();
            return mem;
        }
    };//end of class.
};// namespace mg