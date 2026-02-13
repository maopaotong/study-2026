#pragma once
#include "Entity.h"
#include <array>

namespace mg
{

    struct PosColorVertex
    {
        float x;
        float y;
        float z;
        uint32_t abgr;
    };

    struct Entity00 : public Entity
    {

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

            int vCount = 8;

            const PosColorVertex *vertices = new PosColorVertex[vCount]{
                {-1.0f, 1.0f, 1.0f, 0xff000000},
                {1.0f, 1.0f, 1.0f, 0xff0000ff},
                {-1.0f, -1.0f, 1.0f, 0xff00ff00},
                {1.0f, -1.0f, 1.0f, 0xff00ffff},
                {-1.0f, 1.0f, -1.0f, 0xffff0000},
                {1.0f, 1.0f, -1.0f, 0xffff00ff},
                {-1.0f, -1.0f, -1.0f, 0xffffff00},
                {1.0f, -1.0f, -1.0f, 0xffffffff},
            };
            int iCount = 3 * 12;
            const uint16_t *tlist = new uint16_t[iCount]{
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

            vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, vCount * sizeof(PosColorVertex), [](void *mData, void *uData)
                                                         { delete[] static_cast<PosColorVertex *>(mData); }),
                                           vlayout);
            ibh = bgfx::createIndexBuffer(bgfx::makeRef(tlist, iCount * sizeof(uint16_t), [](void *mData, void *uData)
                                                        { delete[] static_cast<uint16_t *>(mData); }));

            bx::mtxScale(mtx1, 0.5f);

            return 0;
        }

        void submit(int viewId) override
        {
            bx::mtxRotateXY(mtx2, counter * 0.01f, counter * 0.01f);
            bx::mtxMul(mtx2, mtx2, mtx1);
            bgfx::setTransform(mtx2);

            counter++;
            Entity::submit(viewId);
        }
    };

};
