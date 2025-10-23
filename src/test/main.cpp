#include "GLWidget.hpp"
#include "ShaderProgram.h"
// #include "Vertex.h"
// #include "Model.hpp"

#include "VertexArray.hpp"

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

class TWidget : public GLWidget
{
    ShaderProgram s{"../glsl/test/test.vs", "../glsl/test/test.fs"};
    VertexArray _va;
    unsigned int VBO;
    unsigned int EBO;
    virtual void application() override
    {

        // unsigned int _ebo;
        // _va.setElementBuffer(indices, 3);
        // glGenBuffers(1, &_ebo);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6, indices, GL_STATIC_DRAW);

        // glGenVertexArrays(1, &VAO);
        // VertexBuffer vb;
        // vb.set_data(sizeof(vertices), vertices);
        // vb.add_layout(GL_FLOAT, 3, true);
        // _va.addVertexBuffer(vb);

        // _va.bind();

        _va.bind();
        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 3);

        _va.attach_buffer(layout, sizeof(vertices), vertices);
        _va.bind();
        // glGenBuffers(1, &VBO);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    }

    virtual void render_loop() override
    {
        _va.bind();
        s.use();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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