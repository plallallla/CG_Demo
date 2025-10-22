#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"

float cubeVertices[] = 
{
    // positions         
    -0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 

    -0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f, 

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f, -0.5f, 

    -0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
};

class UniformWidget : public GLWidget
{
    ShaderProgram _red{"../glsl/uniform/uniform_block.vs", "../glsl/uniform/red.fs"};
    ShaderProgram _yellow{"../glsl/uniform/uniform_block.vs", "../glsl/uniform/yellow.fs"};
    ShaderProgram _blue{"../glsl/uniform/uniform_block.vs", "../glsl/uniform/blue.fs"};
    ShaderProgram _green{"../glsl/uniform/uniform_block.vs", "../glsl/uniform/green.fs"};

    unsigned int uboMatrices;

    VertexArray _va;
    /**
     * @brief 应用程序函数
     * 
     */
    virtual void application() override
    {
        VertexBuffer vb;
        vb.set_data(sizeof(cubeVertices), cubeVertices);
        vb.add_layout(GL_FLOAT, 3, true);
        _va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));
        glEnable(GL_DEPTH_TEST);
        CAMERA.set_position(glm::vec3(0.0f, 0.0f, 5.0f));

        // configure a uniform buffer object
        // ---------------------------------
        // first. We get the relevant block indices
        unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(_red.get_id(), "Matrices");
        unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(_green.get_id(), "Matrices");
        unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(_blue.get_id(), "Matrices");
        unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(_yellow.get_id(), "Matrices");
        // then we link each shader's uniform block to this uniform binding point
        glUniformBlockBinding(_red.get_id(), uniformBlockIndexRed, 0);
        glUniformBlockBinding(_green.get_id(), uniformBlockIndexGreen, 0);
        glUniformBlockBinding(_blue.get_id(), uniformBlockIndexBlue, 0);
        glUniformBlockBinding(_yellow.get_id(), uniformBlockIndexYellow, 0);
        // Now actually create the buffer
        glGenBuffers(1, &uboMatrices);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // define the range of the buffer that links to a uniform binding point
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

        // store the projection matrix (we only do this once now) (note: we're not using zoom anymore by changing the FoV)
        glm::mat4 projection = glm::perspective(45.0f, (float)_width / (float)_height, 0.1f, 100.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(CAMERA.get_view_matrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 1);
    }
    /**
     * @brief 在渲染循环中执行
     * 
     */
    virtual void render_loop() override
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(CAMERA.get_view_matrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        _va.bind();
        _red.use();
        glm::mat4 model = glm::mat4(1.0f);
        {
            _red.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
            _red.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        {
            _yellow.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
            _yellow.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        {
            _blue.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
            _blue.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        {
            _green.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
            _green.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
public:
    UniformWidget(int w, int h, std::string_view title) : GLWidget(w,h,title) {}
};

int main()
{
    UniformWidget w{800, 600, "u"};
    w.render();
    return 0;
}