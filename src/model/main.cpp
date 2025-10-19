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
    ShaderProgram sp;
    Model ourModel{"../resources/backpack/backpack.obj"};
    virtual void application()
    {
        glEnable(GL_DEPTH_TEST);
        sp.load_vs_file("../glsl/model/model.vs");
        sp.load_fs_file("../glsl/model/model.fs");
        sp.link();
    }
    virtual void render_loop()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sp.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = CAMERA.get_view_matrix();
        projection = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 100.0f);
        unsigned int modelLoc = glGetUniformLocation(sp.get_id(), "model");
        unsigned int viewLoc  = glGetUniformLocation(sp.get_id(), "view");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        sp.set_uniform<glm::mat4>("projection", projection);
        ourModel.Draw(sp);
    }
};

int main()
{
    MoedlWidget widget(800, 600, "model");
    widget.render();
    return 0;
}