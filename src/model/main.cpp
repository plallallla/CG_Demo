#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Camera.hpp"
#include "GLWidget.hpp"

class MoedlWidget : public GLWidget
{
public:
    MoedlWidget(int w, int h, std::string_view title) : GLWidget(w,h,title) {}
private:
    ShaderProgram sp1{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    ShaderProgram sp2{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    Model ourModel{"../resources/backpack/backpack.obj"};
    virtual void application()
    {
        glEnable(GL_DEPTH_TEST);
        CAMERA.set_position({0,0,10});
    }
    virtual void render_loop()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sp1.use();
        glm::mat4 model = glm::mat4(1.0f);
        sp1.set_uniform("model", model);
        sp1.set_uniform("view", CAMERA.get_view_matrix());
        sp1.set_uniform("projection", get_projection());
        ourModel.render_elements(sp1);

        // glUseProgram(0);
        sp2.use();

        // sp2.use();
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0, 2, 0));
        // sp2.set_uniform("model", model);
        // sp2.set_uniform("view", CAMERA.get_view_matrix());
        // sp2.set_uniform("projection", get_projection());
        // ourModel.Draw(sp2);
    }
};

int main()
{
    MoedlWidget widget(800, 600, "model");
    widget.render();
    return 0;
}