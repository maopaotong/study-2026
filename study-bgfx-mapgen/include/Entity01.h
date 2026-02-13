#pragma once
#include "Entity.h"
#include "Common.h"
#include "DualMesh.h"
#include "DualMap.h"
#include "ElevationGen.h"
#include "MapGen.h"
namespace mg
{

    struct Entity01 : public Entity
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
            unsigned int vSize = 3 + 2 + 2; //

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

            unsigned int *iData = new unsigned int[iCount];
            int iIdx = 0;
            int len = mesh.numSolidSides;
            for (int s = 0; s < len; s++)
            {
                iData[len - iIdx - 1] = mesh._triangles[s];
                iIdx++;
            }

            vlayout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
                .end();

            vbh = bgfx::createVertexBuffer(bgfx::makeRef(vData, sizeof(Vertex)*vCount, [](void * mData, void * uData){ delete[] static_cast<Vertex*>(mData); }), vlayout);
            ibh = bgfx::createIndexBuffer(bgfx::makeRef(iData, sizeof(unsigned int) * iCount,[](void * mData, void * uData){ delete[] static_cast<unsigned int*>(mData); }));
            bx::mtxScale(mtx1, 0.5f);            

            uHandle = bgfx::createUniform("s_colorMap", bgfx::UniformType::Sampler);
            texHandle = ColorMap::createTexture();
            return 0;
        }
        void submit(int viewId) override
        {
            bx::mtxRotateXY(mtx2, counter * 0.01f, counter * 0.01f);
            bx::mtxMul(mtx2, mtx2, mtx1);
            bgfx::setTransform(mtx2);
            //
            bgfx::setTexture(0, uHandle, texHandle);
            counter++;
            Entity::submit(viewId);
        }
    };

};
