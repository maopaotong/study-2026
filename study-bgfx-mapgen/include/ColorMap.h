#pragma once
#include <bgfx/bgfx.h>
namespace mg
{

    struct ColorMap
    {

        static bgfx::TextureHandle createTexture()
        {
            const int cols = 64;
            const int rows = 64;

            const bgfx::Memory *mem = bgfx::alloc(cols * rows * 4);
            unsigned char *pixels = mem->data;
            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {

                    float e = 2.0f * static_cast<float>(x) / cols - 1.0f;
                    float m = static_cast<float>(y) / rows;

                    float r, g, b;

                    if (x == cols / 2 - 1)
                    {
                        r = 48;
                        g = 120;
                        b = 160;
                    }
                    else if (x == cols / 2 - 2)
                    {
                        r = 48;
                        g = 100;
                        b = 150;
                    }
                    else if (x == cols / 2 - 3)
                    {
                        r = 48;
                        g = 80;
                        b = 140;
                    }
                    else if (e < 0.0)
                    {
                        r = 48 + 48 * e;
                        g = 64 + 64 * e;
                        b = 127 + 127 * e;
                    }
                    else
                    {                    // adapted from terrain-from-noise article
                        m = m * (1 - e); // higher elevation holds less moisture; TODO: should be based on slope, not elevation

                        r = 210 - 100 * m;
                        g = 185 - 45 * m;
                        b = 139 - 45 * m;
                        r = 255 * e + r * (1 - e),
                        g = 255 * e + g * (1 - e),
                        b = 255 * e + b * (1 - e);
                    }
                    int p = (y * cols + x) * 4;
                    pixels[p++] = static_cast<unsigned char>(r);
                    pixels[p++] = static_cast<unsigned char>(g);
                    pixels[p++] = static_cast<unsigned char>(b);
                    pixels[p++] = 255;
                } // end for
            } // end for

            bgfx::TextureHandle handle = bgfx::createTexture2D(cols, rows, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT, mem);

            return handle;
        }

    }; // end of struct
}; // end of namespace