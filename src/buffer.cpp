#include <painter2/buffer.hpp>

/// @brief reset the buffer data. This function should only be called once
/// @param stride is the column size of the data array
/// @param maxSize is the maximum capacity of the buffer
void pnt::BufferData::reset(const int& stride, const int &maxSize)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    column = stride;
    int _stride = sizeof(float) * column;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _stride * maxSize, nullptr, GL_DYNAMIC_DRAW);

    data.clear();
    data.reserve(column * maxSize);

    // x, y, r, g, b, a

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, _stride, (void*)0); // position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, _stride, (void*)(2 * sizeof(float))); // position
}


void pnt::BufferData::use()
{
}