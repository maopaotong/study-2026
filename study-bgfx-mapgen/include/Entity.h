#pragma once
#include <iostream>
#include <bgfx/bgfx.h>

namespace mg
{
    
    struct Entity
    {
        std::string shader;
        bgfx::ProgramHandle program;
        bgfx::VertexLayout vlayout;
        bgfx::VertexBufferHandle vbh;
        bgfx::IndexBufferHandle ibh;
        Entity(std::string shader) : shader(shader)
        {
        }
        virtual int init()
        {
            program = ShaderUtil::loadProgram(shader, shader, "glsl");
            if (!bgfx::isValid(program))
            {
                LogUtil::log("Failed to load program!");
                return -1;
            }
            return 0;
        }

        void submit(int viewId)
        {

            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);
            bgfx::submit(viewId, program);
        }
        void destroy()
        {
            bgfx::destroy(vbh);
            bgfx::destroy(ibh);
            bgfx::destroy(program);
        }
    };

};
