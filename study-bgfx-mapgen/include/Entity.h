#pragma once
#include <iostream>
#include <bgfx/bgfx.h>
#include "Renderable.h"
namespace mg
{

    struct Entity : public Renderable
    {
        std::string shader;
        bgfx::ProgramHandle program;
        bgfx::VertexLayout vlayout;
        bgfx::VertexBufferHandle vbh;
        bgfx::IndexBufferHandle ibh;

        unsigned int counter = 0;
        float mtx1[16];
        float mtx2[16];

        Entity(std::string shader) : shader(shader)
        {
        }
        ~Entity()
        {
        }
        virtual int init() override
        {
            program = ShaderUtil::loadProgram(shader, shader, "glsl");
            if (!bgfx::isValid(program))
            {
                LogUtil::log("Failed to load program!");
                return -1;
            }
            return 0;
        }

        virtual void submit(int viewId) override
        {
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);
            bgfx::submit(viewId, program);
        }
        virtual void destroy() override
        {
            bgfx::destroy(program);
            bgfx::destroy(vbh);
            bgfx::destroy(ibh);
        }
    };

};
