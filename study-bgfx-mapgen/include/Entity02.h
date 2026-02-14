#pragma once
#include "renderable.h"
#include "mapgen.h"

#define DEBUG_SHOW_TRIANGLES_COUNT 100

namespace mg
{


    struct Entity02 : public Entity
    {
        struct Vertex
        {
            float x;
            float y;
            float z;
            float u0;
            float v0;
            float u1;
            float v1;
        };

        bgfx::TextureHandle texHandle;
        bgfx::UniformHandle uHandle;

        Entity02() : Entity("s02")
        {
        }
        int init() override
        {
            int err = Entity::init();
            if (err)
            {
                return err;
            }

            Args mArgs;
            DualMesh::Data data = MapGen::generateDualData(mArgs);
            DualMesh mesh(data);
            DualMap map(mesh);
            float island = 0.5;
            ElevationGen::Constraints constraints(128, island);
            ElevationGen::Args eArgs;
            ElevationGen eGen{mesh, map.elevation_t, map.elevation_r};
            eGen.assignElevation(constraints, eArgs);

            std::vector<float> &elevation_r = map.elevation_r;

            unsigned int vCount = mesh.numRegions;
            unsigned int iCount = mesh.numSolidSides;
            //
            iCount = 3 * DEBUG_SHOW_TRIANGLES_COUNT;
            Vertex *vData = new Vertex[vCount];

            for (int r = 0; r < mesh.numRegions; r++)
            {
                int vIdx = r;
                vData[vIdx].x = mesh.x_of_r(r);
                vData[vIdx].y = mesh.y_of_r(r);
                vData[vIdx].z = elevation_r[r];
                vData[vIdx].u0 = mesh.x_of_r(r);
                vData[vIdx].v0 = mesh.y_of_r(r);
                vData[vIdx].u1 = mesh.x_of_r(r);
                vData[vIdx].v1 = mesh.y_of_r(r);
            } //

            uint16_t *iData = new uint16_t[iCount];
            

            for (int i = 0; i < iCount; i++)
            {
                iData[i] = mesh._triangles[i];

                std::cout << iData[i] << "(" << vData[iData[i]].x << "," << vData[iData[i]].y << "),";
                if (i % 3 == 2)
                {
                    std::cout << std::endl;
                }
            }

            vlayout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
                .end();

            vbh = bgfx::createVertexBuffer(bgfx::makeRef(vData, sizeof(Vertex) * vCount, [](void *mData, void *uData)
                                                         { delete[] static_cast<Vertex *>(mData); }),
                                           vlayout);
            ibh = bgfx::createIndexBuffer(bgfx::makeRef(iData, sizeof(uint16_t) * iCount, [](void *mData, void *uData)
                                                        { delete[] static_cast<uint16_t *>(mData); }));
            bx::mtxScale(mtx1, 1.0f, 1.0f, 10.0f); //

            uHandle = bgfx::createUniform("s_colorMap", bgfx::UniformType::Sampler);
            texHandle = ColorMap::createTexture();
            return 0;
        }
        void submit(int viewId) override
        {
            bx::mtxRotateXY(mtx2, counter * 0.01f, counter * 0.01f);
            //bx::mtxRotateXY(mtx2, 0 * 0.01f, 0 * 0.01f);
            bx::mtxMul(mtx3, mtx2, mtx1);
            bgfx::setTransform(mtx3);
            //
            // bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_LINES);

            bgfx::setTexture(0, uHandle, texHandle);
            uint64_t state = 0                            //
                             | BGFX_STATE_WRITE_RGB       //
                             | BGFX_STATE_WRITE_A         //
                             | BGFX_STATE_WRITE_Z         //
                             | BGFX_STATE_DEPTH_TEST_LESS //
                             | BGFX_STATE_CULL_CCW        // TODO use CULL_CW
                                                          //| BGFX_STATE_MSAA            //
                                                          //| BGFX_STATE_PT_POINTS //
                ;
            bgfx::setState(state);
            Entity::submit(viewId);
            counter++;
        }
    };

};
