#include <painter2/core.hpp>


pnt::Painter2::Painter2(const std::string &t, const int &w, const int &h)
{
    title = t;
    width = w;
    height = h;
}


bool pnt::Painter2::start()
{
    if(!initGLFW()) return false;

    unsigned int basicShader;
    compileBasicShader(vertexShaderSrc, fragmentShaderSrc, basicShader);

    if(!onReady()) return false;

    int mProjLocation;
    mProjLocation = glGetUniformLocation(basicShader, "projectionMatrix");
    glUseProgram(basicShader);
    auto projMat = glm::ortho<float>(0.0f, width, height, 0.0f, 100.0f, -100.0f);
    glUniformMatrix4fv(mProjLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    glViewport(0, 0, width, height);

    // reset buffers for use
    modelMatrices.push_back(glm::mat4(1.0f));
    basicShapeBuffer.reset(6, 2000);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        beginUseBuffer();
        onRender();
        endUseBuffer();
        glfwSwapBuffers(window);

        // This makes it safe to use save() function without calling restore()
        // otherwise, the modelMatrices will get larger as matrices keep added every frame
        while(modelMatrices.size() > 1) modelMatrices.pop_back();
    }

    return true;
}

const std::string &pnt::Painter2::getError() const
{
    return error;
}

pnt::Painter2::~Painter2()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}


const int &pnt::Painter2::getWidth() const
{
    return width;
}

const int &pnt::Painter2::getHeight() const
{
    return height;
}

void pnt::Painter2::setError(const std::string &msg)
{
    error = msg;
}

void pnt::Painter2::beginUseBuffer(BufferData *buffer)
{
    if(!buffer) {
        currentBuffer = &basicShapeBuffer;
    } else
    currentBuffer = buffer;

    currentBuffer->data.clear();
}

void pnt::Painter2::endUseBuffer()
{
    if(!currentBuffer) return;
    glBindBuffer(GL_ARRAY_BUFFER, currentBuffer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, currentBuffer->data.size() * sizeof(float), currentBuffer->data.data());
    glBindVertexArray(currentBuffer->vao);
    glDrawArrays(GL_TRIANGLES, 0, currentBuffer->data.size() / currentBuffer->column);
    currentBuffer = nullptr;
}


void pnt::Painter2::setRenderColor(const float& r, const float& g, const float& b, const float& a)
{
    fillColor.x = r;
    fillColor.y = g;
    fillColor.z = b;
    fillColor.w = a;
}

void pnt::Painter2::setRenderLineWidth(const float &width)
{
    strokeWidth = width;
}

void pnt::Painter2::renderFillRect(const float &x, const float &y, const float &w, const float &h)
{   
    glm::vec2 p0{ x, y },
        p1{ x + w, y },
        p2{ x + w, y + h },
        p3{ x, y + h };

    pushVertex(p0); pushVertex(p1); pushVertex(p2); 
    pushVertex(p0); pushVertex(p2); pushVertex(p3);
}

void pnt::Painter2::renderStrokeRect(const float &x, const float &y, const float &w, const float &h)
{
    renderFillRect(x, y, w, strokeWidth);
    renderFillRect(x, y + h, w, strokeWidth);
    renderFillRect(x, y, strokeWidth, h);
    renderFillRect(x + w, y, strokeWidth, h);
}

void pnt::Painter2::renderLine(const float &x1, const float &y1, const float &x2, const float &y2)
{
    glm::vec2 p0{ x1, y1 };
    glm::vec2 p1{ x2, y2 };

    auto dir = glm::normalize(p1 - p0);
    auto normal = glm::vec2(-dir.y, dir.x);
    auto offset = normal * (strokeWidth * 0.5f);

    glm::vec2 v0 = p0 + offset;
    glm::vec2 v1 = p1 + offset;
    glm::vec2 v2 = p1 - offset;
    glm::vec2 v3 = p0 - offset;

    pushVertex(v0); pushVertex(v1); pushVertex(v2); 
    pushVertex(v0); pushVertex(v2); pushVertex(v3);
}


void pnt::Painter2::renderFillArc(const float& x, const float& y, const float& r, const float& startAngle, const float& endAngle)
{
    int step = 20;
    glm::vec2 origin{ x, y };
    glm::vec2 prevPos { x + r, y };

    for(int i = 0; i <= 360; i += step)
    {
        auto angle = glm::radians((float)i);
        glm::vec2 pos{ x + std::cos(angle) * r, y + std::sin(angle) * r };
        pushVertex(origin);
        pushVertex(prevPos);
        pushVertex(pos);

        prevPos = pos;
    }
}


void pnt::Painter2::save(glm::mat4* m)
{
    if(!m) modelMatrices.push_back(glm::mat4(1.0f));
    else modelMatrices.push_back(*m);
}

void pnt::Painter2::restore()
{
    if(modelMatrices.size() <= 1) return;
    modelMatrices.pop_back();
}

void pnt::Painter2::setTranslation(const float &x, const float &y)
{
    modelMatrices.back() = glm::translate(modelMatrices.back(), glm::vec3(x, y, 0.0f));
}

void pnt::Painter2::setRotation(const float &angleInRadians)
{
    modelMatrices.back() = glm::rotate(modelMatrices.back(), angleInRadians, glm::vec3(0, 0, 1));
}

void pnt::Painter2::setScale(const float &sx, const float &sy)
{
    modelMatrices.back() = glm::scale(modelMatrices.back(), glm::vec3(sx, sy, 0));
}


void pnt::Painter2::pushVertex(const glm::vec2& pos)
{
    auto tPos = modelMatrices.back() * glm::vec4(pos, 0, 1.0f);
    currentBuffer->data.push_back(tPos.x);
    currentBuffer->data.push_back(tPos.y);
    // color
    currentBuffer->data.push_back(fillColor.x);
    currentBuffer->data.push_back(fillColor.y);
    currentBuffer->data.push_back(fillColor.z);
    currentBuffer->data.push_back(fillColor.w);
}


bool pnt::Painter2::initGLFW()
{
    if(!glfwInit()) {
        setError("Unable to initialize glfw");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (!window) {
        setError("Unable to create window context for opengl4.4");
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        setError("Unable to load GLAD.c");
        return false;
    }

    return true;
}


void pnt::Painter2::compileBasicShader(const char* vShader, const char* fShader, unsigned int& program)
{
    auto compile = [](GLenum type, const char* src) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    };

    unsigned int vs = compile(GL_VERTEX_SHADER, vShader);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fShader);
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
}