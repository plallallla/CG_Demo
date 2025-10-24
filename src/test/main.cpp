#include "GLWidget.hpp"
#include "ShaderProgram.h"
// #include "Vertex.h"
// #include "Model.hpp"


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

    unsigned int VBO;
    GLuint va1, va2;
    GLuint eb1, eb2;
    // GLuint VAO;
    // GLuint EBO;

    // VertexArray vao1, vao2;

    virtual void application() override
    {

        // BufferLayout layout;
        // layout.add_attribute(GL_FLOAT, 3);
        // vao1.attach_vertex_buffer(layout, VBO);
        // vao1.attach_element_buffer(eb1);


        VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
        eb1 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1);
        eb2 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2);

        glGenVertexArrays(1, &va1);
        glBindVertexArray(va1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb1);
        glBindVertexArray(0);

        glGenVertexArrays(1, &va2);
        glBindVertexArray(va2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb2);
        glBindVertexArray(0);

    }

    virtual void render_loop() override
    {
        s.use();

        glBindVertexArray(va1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(va2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
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