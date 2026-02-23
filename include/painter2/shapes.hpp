#ifndef __BYTENOL_PAINTER2_SHAPES_HPP__
#define __BYTENOL_PAINTER2_SHAPES_HPP__

#include "buffer.hpp"


namespace pnt
{
    struct Rect
    {
        float x, y, w, h;
        Rect() = default;
    };

    void renderFillRect(const float& x, const float& y, const float& w, const float& h);
    // void renderFillRect(const Rect& r);
}

#endif 
