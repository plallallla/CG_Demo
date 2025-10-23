#include "Camera.hpp"
#include "Model.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"
#include <GLFW/glfw3.h>
#include <memory>

float points[] = 
{
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

class GeometryWidget : public GLWidget
{
public:
    GeometryWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    VertexArray _va;
    // ShaderProgram _s{"../glsl/geometry/geo.vs", "../glsl/geometry/geo.fs"};
    ShaderProgram _s{"../glsl/geometry/geo.vs", "../glsl/geometry/geo.gs", "../glsl/geometry/geo.fs"};
    ShaderProgram _explode{"../glsl/geometry/explode.vs", "../glsl/geometry/explode.gs", "../glsl/geometry/explode.fs"};
    Model _model{"../resources/backpack/backpack.obj"};

    virtual void application() override
    {
        VertexBuffer vb;
        vb.set_data(sizeof(points), points);
        vb.add_layout(GL_FLOAT, 2, true);
        vb.add_layout(GL_FLOAT, 3, true);
        _va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));
        glEnable(GL_DEPTH_TEST);
        _explode.use();
        glm::mat4 model(1.0);
        _explode.set_uniform("model", model);
        _explode.set_uniform("projection", get_projection());
    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // _va.bind();
        // _s.use();
        // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // glDrawArrays(GL_POINTS, 0, 4);
        _explode.use();
        _explode.set_uniform("view", CAMERA.get_view_matrix());
        _explode.set_uniform("time", static_cast<float>(glfwGetTime()));
        _model.Draw(_explode);
    }
};

int main()
{
    GeometryWidget w{800, 600, "g"};
    w.render();
    return 0;
}