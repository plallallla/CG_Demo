#include "GLWidget.hpp"
#include "Shader.hpp"
#include "ShaderProgram.h"
#include "Texture.hpp"
#include "Vertex.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <string>


const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

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

float tri[] = 
{
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};

float tri_pos_data[] = 
{
    // positions       
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

float tri_col_data[] = 
{
    // colors
    1.0f, 0.0f, 0.0f,  // bottom right
    0.0f, 1.0f, 0.0f,  // bottom left
    0.0f, 0.0f, 1.0f   // top 
};

class InstanceWidget : public GLWidget
{
public:
    InstanceWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    ShaderProgram _quad{"../glsl/instance/quad.vs", "../glsl/instance/quad.fs"};
    VertexArray _quad_va;
    unsigned int quadVAO, quadVBO;

    VertexArray _triangle;
    ShaderProgram _tri_sp;

    virtual void application() override
    {
        _triangle.bind();
        // VertexBuffer tri_vb;
        // tri_vb.set_data(sizeof(tri), tri);
        // tri_vb.add_layout(GL_FLOAT, 3, true);
        // tri_vb.add_layout(GL_FLOAT, 3, true);
        VertexBuffer tri_pos;
        tri_pos.set_data(sizeof(tri_pos_data), tri_pos_data);
        tri_pos.add_layout(GL_FLOAT, 3, true);
        _triangle.addVertexBuffer(tri_pos);

        VertexBuffer tri_col;
        tri_col.set_data(sizeof(tri_col_data), tri_col_data);
        tri_col.add_layout(GL_FLOAT, 3, true);
        _triangle.addVertexBuffer(tri_col);

        _tri_sp.use();
        _tri_sp.load_vs_src(vertexShaderSource);
        _tri_sp.load_fs_src(fragmentShaderSource);
        _tri_sp.link();

        // VertexBuffer quad_vb;
        // quad_vb.set_data(sizeof(quadVertices), quadVertices);
        // quad_vb.add_layout(GL_FLOAT, 2, true);
        // quad_vb.add_layout(GL_FLOAT, 3, true);
        // _quad_va.addVertexBuffer(std::make_shared<VertexBuffer>(quad_vb));
        // // _quad.use();
        // int index = 0;
        // float offset = 0.1f;
        // glm::vec2 translations[100];
        // for(int y = -10; y < 10; y += 2)
        // {
        //     for(int x = -10; x < 10; x += 2)
        //     {
        //         glm::vec2 translation;
        //         translation.x = (float)x / 10.0f + offset;
        //         translation.y = (float)y / 10.0f + offset;
        //         // std::string key = std::string{"offsets["} + std::to_string(index++) + std::string{"]"};
        //         // _quad.set_uniform(key, translation);
        //         translations[index++] = translation;
        //     }
        // }
        // VertexBuffer index_vb;
        // index_vb.set_data(100, &translations[0]);
        // index_vb.add_layout(GL_FLOAT, 2, true);
        // _quad_va.addVertexBuffer(std::make_shared<VertexBuffer>(index_vb));
        // glm::vec2 translations[100];
        // int index = 0;
        // float offset = 0.1f;
        // for (int y = -10; y < 10; y += 2)
        // {
        //     for (int x = -10; x < 10; x += 2)
        //     {
        //         glm::vec2 translation;
        //         translation.x = (float)x / 10.0f + offset;
        //         translation.y = (float)y / 10.0f + offset;
        //         translations[index++] = translation;
        //     }
        // }
        // // store instance data in an array buffer
        // // --------------------------------------
        // unsigned int instanceVBO;
        // glGenBuffers(1, &instanceVBO);
        // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // // set up vertex data (and buffer(s)) and configure vertex attributes
        // // ------------------------------------------------------------------
        // float quadVertices[] = {
        //     // positions     // colors
        //     -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        //     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        //     -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
        //     -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        //     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        //     0.05f,  0.05f,  0.0f, 1.0f, 1.0f
        // };
        // glGenVertexArrays(1, &quadVAO);
        // glGenBuffers(1, &quadVBO);
        // glBindVertexArray(quadVAO);
        // glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        // // also set instance data
        // glEnableVertexAttribArray(2);
        // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
    }
    virtual void render_loop() override
    {
        _triangle.bind();
        _tri_sp.use();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // _quad.use();
        // _quad_va.bind();
        // glBindVertexArray(quadVAO);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    }
};

int main()
{
    InstanceWidget w{ 800, 600, "instance" };
    w.render();
    return 0;
}