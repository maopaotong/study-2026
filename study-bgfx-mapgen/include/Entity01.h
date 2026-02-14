#pragma once
#include "renderable.h"
#include "Common.h"
#include <array>

namespace mg
{

    struct Entity01 : public Entity
    {

        Entity01() : Entity("s01")
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
                {0.0f, 0.0f, 00.0f, 0xff, 0xff, 0xff, 0xff}, // 0
                {1.0f, 0.0f, 00.0f, 0xff, 0x00, 0x00, 0xff}, // 1 r
                {0.0f, 1.0f, 00.0f, 0x00, 0xff, 0x00, 0xff}, // 2 g
                {0.0f, 0.0f, -1.0f, 0x00, 0x00, 0xff, 0xff}, // 3 b
                {1.0f, 1.0f, 00.0f, 0xff, 0xff, 0x00, 0xff}, // 4
                {1.0f, 0.0f, -1.0f, 0xff, 0x00, 0xff, 0xff}, // 5
                {0.0f, 1.0f, -1.0f, 0x00, 0xff, 0xff, 0xff}, // 6
                {1.0f, 1.0f, -1.0f, 0x00, 0x00, 0x00, 0xff}, // 7
            };
            /*                              ^
                  6--------7              y |
                 /|       /|                |
                2--------4 |                |
                | |      | |                |             x
                | 3------|-5                +-------------->
                |/       |/                /
                0--------1                z
            */
            int iCount = 3 * 12;
            const uint16_t *tlist = new uint16_t[iCount]{
                // clang-format off
                0, 4, 2,
                0, 1, 4,
                1, 7, 4,
                1, 5, 7,
                2, 7, 6,
                2, 4, 7,//
                5, 6, 7,
                5, 3, 6,
                3, 2, 6,
                3, 0, 2,
                0, 3, 5,
                0, 5, 1,
                /*
                */
                // clang-format on
            };

            vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, vCount * sizeof(PosColorVertex), [](void *mData, void *uData)
                                                         { delete[] static_cast<PosColorVertex *>(mData); }),
                                           vlayout);
            ibh = bgfx::createIndexBuffer(bgfx::makeRef(tlist, iCount * sizeof(uint16_t), [](void *mData, void *uData)
                                                        { delete[] static_cast<uint16_t *>(mData); }));

            bx::mtxScale(mtx1, 10.0f, 10.0f, 10.0f);            
            bx::mtxIdentity(mtx2);            
            bx::mtxMul(mtx3, mtx2, mtx1);
            return 0;
        }
        
        void submit(int viewId) override
        {
            //bx::mtxRotateXY(mtx2, counter * 0.01f, counter * 0.01f);
            bgfx::setTransform(mtx3);

            // bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_LESS, 0);

            uint64_t state = 0                            //
                             | BGFX_STATE_WRITE_RGB       //
                             | BGFX_STATE_WRITE_A         //
                             | BGFX_STATE_WRITE_Z         //
                             | BGFX_STATE_DEPTH_TEST_LESS //
                             | BGFX_STATE_CULL_CW        //
                //| BGFX_STATE_MSAA            //
                //| BGFX_STATE_PT_POINTS //
                ;
            bgfx::setState(state);
            Entity::submit(viewId);
            counter++;
        }
    };

};
