#pragma once
#include <cmath>

namespace mg
{
    struct Args
    {
        int meshWidth;

        double spacing = 5.5;
        double mountainSpacing = 35;
        double boundarySpacing = spacing * std::sqrt(2);
        int mountainRetries = 30;
        int retries = 6;
        unsigned int seed = 12345;
        //
        Args() : meshWidth(1000) //
        {

        };
    };
}