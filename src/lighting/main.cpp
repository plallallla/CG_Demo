#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"
#include "Texture.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

float vertices[] = 
{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 cubePositions[] = 
{
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

class LightWidget : public GLWidget
{
    ShaderProgram _shader{"../glsl/lighting/lighting.vs" ,"../glsl/lighting/lighting.fs"};
    ShaderProgram _lamp_shader{"../glsl/lighting/lamp.vs" ,"../glsl/lighting/lamp.fs"};
    VertexArray _va;
    glm::vec3 _light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 _light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int spec_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2_specular.png");
    unsigned int diff_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2.png");
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);

        VertexBuffer vb;
        vb.set_data(sizeof(vertices), vertices);
        vb.add_layout(GL_FLOAT, 3, true);
        vb.add_layout(GL_FLOAT, 3, true);
        vb.add_layout(GL_FLOAT, 2, true);
        _va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));
        _shader.use();
        _shader.add_sampler("material.diffuse", diff_texture);
        _shader.add_sampler("material.specular", spec_texture);
    }

    virtual void render_loop() override
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _va.bind();
        _shader.use();
        _shader.active_samplers();
        _shader.set_uniform("view", CAMERA.get_view_matrix());
        _shader.set_uniform("projection", get_perspective());
        _shader.set_uniform("view_positon", CAMERA.get_position());

        // light attribute
        _shader.set_uniform("light.position", _light_pos);
        _shader.set_uniform("light.direction", CAMERA.get_front());
        _shader.set_uniform("light.ambient", glm::vec3(.1f, .1f, .1f));
        _shader.set_uniform("light.diffuse", glm::vec3(.8f, .8f, .8f));
        _shader.set_uniform("light.specular", glm::vec3(1.f, 1.f, 1.f));
        _shader.set_uniform("light.cutOff",   glm::cos(glm::radians(12.5f)));
        _shader.set_uniform("light.outerCutOff", glm::cos(glm::radians(20.f)));
        _shader.set_uniform("light.constant", 1.0f);
        _shader.set_uniform("light.linear", 0.09f);
        _shader.set_uniform("light.quadratic", 0.032f);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            _shader.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        _lamp_shader.use();
        glm::mat4 model(1.0f);
        model = glm::translate(model, _light_pos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        _lamp_shader.set_uniform("model", model);
        _lamp_shader.set_uniform("view", CAMERA.get_view_matrix());
        _lamp_shader.set_uniform("projection", get_perspective());
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
public:
    LightWidget(int width, int height, std::string_view title) : GLWidget(width, height, title) {}
};

int main()
{
    LightWidget widget(800, 600, "lighting");
    widget.render();
    return 0;
}
