#ifndef __BYTENOL_PAINTER2_BUFFER_HPP__
#define __BYTENOL_PAINTER2_BUFFER_HPP__


#include <vector>
#include <glad/glad.h>

namespace pnt
{
    class BufferData
    {
    private:

    public:
        unsigned int vao, vbo;
        std::vector<float> data;
        int column;
        BufferData() = default;
        void reset(const int& stride, const int& maxSize);
        void use();
    };
}

#endif 