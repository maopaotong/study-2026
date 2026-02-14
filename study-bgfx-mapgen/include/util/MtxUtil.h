
#pragma once
#include <bx/math.h>

namespace mg
{
    struct MtxUtil
    {

        /*
        re-write function from bx, bx::mtxLookAt(view, eye, at, up, handedness);
        bx::mtxLookAt does not work properly, x point to left side of the screen.
        below method work with setState with mask: BGFX_STATE_CULL_CW, it's saying that CCW-order of triangle vertex index data.

                      y ^
                        |
                        |
                        +--------> x
                     ↙   
                  ↙ 
              z 

        */

        static void mtxLookAt(float *_result, const bx::Vec3 &_eye, const bx::Vec3 &_at, const bx::Vec3 &_up)
        {

            const bx::Vec3 forward = bx::normalize(sub(_at, _eye));
            bx::Vec3 right = bx::normalize(cross(forward, _up));
            bx::Vec3 up = bx::cross(right, forward);
            /* clang-format off */
            _result[0] = right.x;       _result[1] = up.x;      _result[2] = forward.x;         _result[3] = 0.0f;
            _result[4] = right.y;       _result[5] = up.y;      _result[6] = forward.y;         _result[7] = 0.0f;
            _result[8] = right.z;       _result[9] = up.z;      _result[10] = forward.z;        _result[11] = 0.0f;
            _result[12] = -dot(right, _eye);    _result[13] = -dot(up, _eye);       _result[14] = -dot(forward, _eye);      _result[15] = 1.0f;
            /* clang-format on */
        }
    };
};