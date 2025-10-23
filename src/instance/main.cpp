#include "GLWidget.hpp"
#include "Shader.hpp"
#include "ShaderProgram.h"
#include "Texture.hpp"
#include "Vertex.h"
#include <memory>
#include <string>

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
    InstanceWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    ShaderProgram _quad{"../glsl/instance/quad.vs", "../glsl/instance/quad.fs"};
    VertexArray _quad_va;
    virtual void application() override
    {
        VertexBuffer quad_vb;
        quad_vb.set_data(sizeof(quadVertices), quadVertices);
        quad_vb.add_layout(GL_FLOAT, 2, true);
        quad_vb.add_layout(GL_FLOAT, 3, true);
        _quad_va.addVertexBuffer(std::make_shared<VertexBuffer>(quad_vb));

        _quad.use();
        int index = 0;
        float offset = 0.1f;
        for(int y = -10; y < 10; y += 2)
        {
            for(int x = -10; x < 10; x += 2)
            {
                glm::vec2 translation;
                translation.x = (float)x / 10.0f + offset;
                translation.y = (float)y / 10.0f + offset;
                std::string key = std::string{"offsets["} + std::to_string(index++) + std::string{"]"};
                _quad.set_uniform(key, translation);
            }
        }

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