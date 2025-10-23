#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"
#include <glm/ext/vector_float2.hpp>

float quadVertices[] = 
{
    // 位置          // 颜色
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f                   
};

class InstanceWidget : public GLWidget
{
public:
    InstanceWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        int index = 0;
        float offset = 0.1f;
        for(int y = -10; y < 10; y += 2)
        {
            for(int x = -10; x < 10; x += 2)
            {
                glm::vec2 translation;
                translation.x = (float)x / 10.0f + offset;
                translation.y = (float)y / 10.0f + offset;
                translations[index++] = translation;
            }
        }
    }
private:
    ShaderProgram _quad{"../glsl/instance/quad.vs", "../glsl/instance/quad.fs"};
    VertexArray _quad_va;
    glm::vec2 translations[100];
    unsigned int quadVAO, quadVBO;
    virtual void application() override
    {
        VertexBuffer quad_vb;
        quad_vb.set_data(sizeof(quadVertices), quadVertices);
        quad_vb.add_layout(GL_FLOAT, 2, true);
        quad_vb.add_layout(GL_FLOAT, 3, true);
        _quad_va.addVertexBuffer(quad_vb);
        VertexBuffer index_vb;
        index_vb.set_data(sizeof(glm::vec2) * 100, &translations);
        index_vb.add_layout(GL_FLOAT, 2, true);
        _quad_va.addVertexBuffer(index_vb);
        _quad_va.bind();
        glVertexAttribDivisor(2, 1);
        _quad_va.unbind();
    }
    virtual void render_loop() override
    {
        _quad.use();
        _quad_va.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    }
};

int main()
{
    InstanceWidget w{ 800, 600, "instance" };
    w.render();
    return 0;
}