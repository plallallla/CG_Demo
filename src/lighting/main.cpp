#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "TextureAttributes.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

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

glm::vec3 pointLightPositions[] = 
{
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

class LightWidget : public GLWidget
{
    ShaderProgram _shader{"../glsl/lighting/lighting.vs" ,"../glsl/lighting/lighting.fs"};
    ShaderProgram _lamp_shader{"../glsl/lighting/lamp.vs" ,"../glsl/lighting/lamp.fs"};
    VertexArray _va;
    glm::vec3 _light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 _light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int spec_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2_specular.png", TEXTURE_2D_RGBA);
    unsigned int diff_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_RGBA);
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);

        _va.attach_vertex_buffer(PNT_LAYOUT, BUFFER.generate_vertex_buffer(sizeof(vertices), vertices));
        _shader.use();
        _shader.set_sampler(0, "material.diffuse");
        _shader.set_sampler(1, "material.specular");
    }

    virtual void render_loop() override
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _va.bind();
        _shader.use();
        _shader.active_sampler(0, diff_texture);
        _shader.active_sampler(1, spec_texture);
        
        // some info
        _shader.set_uniform("viewPos", CAMERA.get_position());
        _shader.set_uniform("material.shininess", 128.f);

        // directional light
        _shader.set_uniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        _shader.set_uniform("dirLight.ambient", glm::vec3(.05f, .05f, .05f));
        _shader.set_uniform("dirLight.diffuse", glm::vec3(.4f, .4f, .4f));
        _shader.set_uniform("dirLight.specular", glm::vec3(.5f, .5f, .5f));

        // point light
        for (int i = 0; i < 4; i++)
        {
            std::string name = "pointLights[" + std::to_string(i) + "]";
            _shader.set_uniform(name + ".position", pointLightPositions[i]);
            _shader.set_uniform(name + ".ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            _shader.set_uniform(name + ".diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            _shader.set_uniform(name + ".specular", glm::vec3(1.0f, 1.0f, 1.0f));
            _shader.set_uniform(name + ".constant", 1.0f);
            _shader.set_uniform(name + ".linear", 0.09f);
            _shader.set_uniform(name + ".quadratic", 0.032f);
        }

        // spot light
        _shader.set_uniform("spotLight.position", _light_pos);
        _shader.set_uniform("spotLight.direction", CAMERA.get_front());
        _shader.set_uniform("spotLight.ambient", glm::vec3(.1f, .1f, .1f));
        _shader.set_uniform("spotLight.diffuse", glm::vec3(.8f, .8f, .8f));
        _shader.set_uniform("spotLight.specular", glm::vec3(1.f, 1.f, 1.f));
        _shader.set_uniform("spotLight.constant", 1.0f);
        _shader.set_uniform("spotLight.linear", 0.09f);
        _shader.set_uniform("spotLight.quadratic", 0.032f);
        _shader.set_uniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        _shader.set_uniform("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        // transform
        _shader.set_uniform("view", CAMERA.get_view_matrix());
        _shader.set_uniform("projection", get_projection());

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
        _lamp_shader.set_uniform("view", CAMERA.get_view_matrix());
        _lamp_shader.set_uniform("projection", get_projection());
        for (int i = 0; i < 4; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            _lamp_shader.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glm::mat4 model(1.0f);
        model = glm::translate(model, _light_pos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        _lamp_shader.set_uniform("model", model);
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
