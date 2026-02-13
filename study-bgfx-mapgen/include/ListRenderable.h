#pragma once
#include <vector>
#include "Renderable.h"
namespace mg
{

    struct ListRenderable : public Renderable
    {
        
        std::vector<Renderable *> list;

        int init() override
        {
            for (Renderable *r : list)
            {
                if (r->init())
                {
                    return -1;
                }
            }
            return 0;
        }
        void submit(int viewId) override
        {
            for (Renderable *r : list)
            {
                r->submit(viewId);
            }
        }
        void destroy() override
        {
            for (Renderable *r : list)
            {
                r->destroy();
            }
        }
    };
};