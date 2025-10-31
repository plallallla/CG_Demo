#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Camera.hpp"
#include "GLWidget.hpp"

#include "FrameBuffer.hpp"

#include "QuadRender.hpp"

class MoedlWidget : public GLWidget
{
public:
    MoedlWidget(int w, int h, std::string_view title) : GLWidget(w,h,title) {}
private:
    ShaderProgram sp1{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    ShaderProgram sp2{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    Model ourModel{"../resources/backpack/backpack.obj"};
    Model cyborg{"../resources/cyborg/cyborg.obj"};

    FrameBuffer frame{};

    GLuint f1{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_RGB)};
    GLuint f2{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_RGB)};

    QuadRender _debug;

    virtual void application()
    {
        glEnable(GL_DEPTH_TEST);
        CAMERA.set_position({0,0,10});

        frame.bind();
        frame.attach_color_texture(0, f1);
        frame.attach_color_texture(1, f2);
        frame.create_render_object(_width*2, _height*2);
        frame.unbind();
    }
    virtual void render_loop()
    {
        frame.bind();

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sp1.use();
        glm::mat4 model = glm::mat4(1.0f);
        sp1.set_uniform("model", model);
        sp1.set_uniform("view", CAMERA.get_view_matrix());
        sp1.set_uniform("projection", get_projection());
        ourModel.render_elements(sp1);
        frame.unbind();

        _debug.render_texture(f1);

    }
};

int main()
{
    MoedlWidget widget(800, 600, "model");
    widget.render();
    return 0;
}