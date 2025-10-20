#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

float vertices[]
{
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

class LightWidget : public GLWidget
{
    ShaderProgram _shader{"../glsl/lighting/lighting.vs" ,"../glsl/lighting/lighting.fs"};
    ShaderProgram _lamp_shader{"../glsl/lighting/lamp.vs" ,"../glsl/lighting/lamp.fs"};
    VertexArray _va;
    glm::vec3 _light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 _light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);

        VertexBuffer vb;
        vb.set_data(sizeof(vertices), vertices);
        vb.add_layout(GL_FLOAT, 3, true);
        vb.add_layout(GL_FLOAT, 3, true);
        _va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));

    }

    virtual void render_loop() override
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _va.bind();
        glm::mat4 model(1.0f);
        _shader.use();
        model = glm::scale(model, {1.0,0.4,0.3});
        _shader.set_uniform("model", model);
        _shader.set_uniform("view", CAMERA.get_view_matrix());
        _shader.set_uniform("projection", get_perspective());
        _shader.set_uniform("light_positon", _light_pos);
        _shader.set_uniform("view_positon", CAMERA.get_position());

        glDrawArrays(GL_TRIANGLES, 0, 36);
        _lamp_shader.use();
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
