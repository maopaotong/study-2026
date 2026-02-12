#pragma once
#include "Entity.h"

namespace mg
{

    struct Entity00 : public Entity
    {
        struct PosColorVertex
        {
            float x;
            float y;
            float z;
            uint32_t abgr;
        };

        static inline PosColorVertex vertices[] = {
            {-1.0f, 1.0f, 1.0f, 0xff000000},
            {1.0f, 1.0f, 1.0f, 0xff0000ff},
            {-1.0f, -1.0f, 1.0f, 0xff00ff00},
            {1.0f, -1.0f, 1.0f, 0xff00ffff},
            {-1.0f, 1.0f, -1.0f, 0xffff0000},
            {1.0f, 1.0f, -1.0f, 0xffff00ff},
            {-1.0f, -1.0f, -1.0f, 0xffffff00},
            {1.0f, -1.0f, -1.0f, 0xffffffff},
        };

        static inline const uint16_t tlist[] = {
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
        Entity00() : Entity("s00")
        {
        }
        int init() override
        {
            int err = Entity::init();
            if (err)
            {
                return err;
            }

            vlayout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();

            vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), vlayout);
            ibh = bgfx::createIndexBuffer(bgfx::makeRef(tlist, sizeof(tlist)));
            return 0;
        }
    };

};
