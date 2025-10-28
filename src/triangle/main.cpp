#include "Buffer.hpp"
#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "TextureAttributes.hpp"
#include "VertexArray.hpp"
#include <glm/fwd.hpp>

    float only_vertices[] = {
    // 位置            
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

class TriangleWidget : public GLWidget
{
    VertexArray _va;
    ShaderProgram _sp{"../glsl/gamma/gamma.vs", "../glsl/gamma/gamma.fs"};
    ShaderProgram _triangle{"../glsl/triangle/triangle.vs", "../glsl/triangle/triangle.fs"};

    virtual void application() override
    {
        GLuint VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices), only_vertices);
        BufferLayout l;
        l.add_attribute(GL_FLOAT, 3);
        _va.attach_vertex_buffer(l, VBO);
    }

    virtual void render_loop() override
    {
        _triangle.use();
        _va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

public:
    TriangleWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};

int main()
{
    TriangleWidget w{800,600,""};
    w.render();
    return 0;
}