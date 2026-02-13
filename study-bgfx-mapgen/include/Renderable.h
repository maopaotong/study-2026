#pragma once

namespace mg
{

    struct Renderable
    {
        virtual int init() = 0;
        virtual void submit(int viewId) = 0;
        virtual void destroy() = 0;
    };

};
