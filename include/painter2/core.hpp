#ifndef __BYTENOL_PAINTER2_CORE_HPP__
#define __BYTENOL_PAINTER2_CORE_HPP__

#include <string>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "buffer.hpp"

namespace pnt
{

    static const char* vertexShaderSrc = R"(
    #version 410 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec4 aColor;
    layout (location = 2) in mat4 mModel;

    uniform mat4 projectionMatrix;

    out vec4 vColor;

    void main() {
        gl_Position = projectionMatrix * vec4(aPos, 0.0, 1.0);
        vColor = aColor;
    }
    )";

    static const char* fragmentShaderSrc = R"(
    #version 410 core
    in vec4 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vColor;
    }
    )";


    class Painter2
    {
    private:
        int width, height;
        std::string title, error;
        GLFWwindow* window = nullptr;

        bool windowShouldClose = false;

        BufferData basicShapeBuffer;
        BufferData* currentBuffer = nullptr;

        glm::vec4 fillColor{ 0, 0, 0, 1 };
        float strokeWidth = 1.0f;

        // transformation matrices
        glm::mat4 mModel;
        std::vector<glm::mat4> modelMatrices;

        glm::vec2 modelScale;

    public:
        Painter2(const std::string& t, const int& w, const int& h);
        bool start();
        const std::string& getError() const;
        virtual ~Painter2();

    protected:
        virtual bool onReady() { return true; }
        virtual bool onProcess() { return true; }
        virtual bool onRender() { return true; }

        const int& getWidth() const;
        const int& getHeight() const;
        void setError(const std::string& msg);

        void beginUseBuffer(BufferData* buffer = nullptr);
        void endUseBuffer();

        void setRenderColor(const float& r, const float& g, const float& b, const float& a = 1.0f);
        void setRenderLineWidth(const float& width);
        void renderFillRect(const float& x, const float& y, const float& w, const float& h);
        void renderStrokeRect(const float& x, const float& y, const float& w, const float& h);
        void renderLine(const float& x1, const float& y1, const float& x2, const float& y2);
        void renderFillArc(const float& x, const float& y, const float& r, const float& startAngle = 0.0f, const float& endAngle = 360.0f);

        // matrix methods

        void save(glm::mat4* m = nullptr);
        void restore();
        void setTranslation(const float& x, const float& y);
        void setRotation(const float& angleInRadians);
        void setScale(const float& sx, const float& sy);

    private:
        void pushVertex(const glm::vec2& pos);
        bool initGLFW();
        void compileBasicShader(const char* vShader, const char* fShader, unsigned int& program);
    };
    

}

#endif 