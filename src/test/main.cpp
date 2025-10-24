#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include <OpenGL/gltypes.h>
// #include "Vertex.h"
// #include "Model.hpp"

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };


    float vertices_with_color[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

    float only_vertices[] = {
    // 位置            
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

    float only_color[] = {// 颜色
    1.0f, 0.0f, 0.0f,   // 右下
    0.0f, 1.0f, 0.0f,   // 左下
    0.0f, 0.0f, 1.0f    // 顶部
};

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

        unsigned int indices_1[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
    };
        unsigned int indices_2[] = {  // note that we start from 0!
        1, 2, 3   // second Triangle
    };


#include "Buffer.hpp"
#include "VertexArray.hpp"

class TWidget : public GLWidget
{
    ShaderProgram s{"../glsl/test/test.vs", "../glsl/test/test.fs"};
    vVertexArray vao1, vao2;

    virtual void application() override
    {

        // GLuint VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
        GLuint VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices_with_color), vertices_with_color);
        GLuint eb1 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1);
        GLuint eb2 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2);

        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 3);
        // layout.add_attribute(GL_FLOAT, 3);

        GLuint ver = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices), only_vertices);
        GLuint col = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_color), only_color);


        vao1.attach_vertex_buffer(layout, ver);
        vao1.attach_vertex_buffer(layout, col);
        // vao1.attach_element_buffer(eb1);

        // vao2.attach_vertex_buffer(layout, VBO);
        // vao2.attach_element_buffer(eb2);

    }

    virtual void render_loop() override
    {
        s.use();

        vao1.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // vao2.bind();
        // glDrawArrays(GL_TRIANGLES, 0, 3);

    }
public:
    TWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};

int main()
{
    TWidget wid{800, 600, "test"};
    wid.render();
    return 0;
}