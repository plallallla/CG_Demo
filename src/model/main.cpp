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

#include "PostRender.hpp"

class MoedlWidget : public GLWidget
{
public:
    MoedlWidget(int w, int h, std::string_view title) : GLWidget(w,h,title) {}
private:
    ShaderProgram sp1{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    ShaderProgram sp2{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    Model ourModel{"../resources/backpack/backpack.obj"};
    Model cyborg{"../resources/cyborg/cyborg.obj"};

    GLuint _wood_texture{TEXTURE_MANAGER.load_texture("../resources/textures/wood.png", TEXTURE_2D_GAMMA)};
    

    FrameBuffer frame{};

    GLuint f1{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_RGB)};
    GLuint f2{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_RGB)};



    virtual void application()
    {
        // glEnable(GL_DEPTH_TEST);
        // CAMERA.set_position({0,0,10});

        // frame.bind();
        // frame.attach_color_texture(0, f1);
        // frame.attach_color_texture(1, f2);
        // frame.create_render_object(_width*2, _height*2);
        // frame.unbind();
    }
    virtual void render_loop()
    {
    }
};

class DebugWidget : public GLWidget
{
public:
    DebugWidget(int w, int h, std::string_view title) : GLWidget(w,h,title) {}
private:

    GLuint _wood_texture{TEXTURE_MANAGER.load_texture("../resources/textures/wood.png", TEXTURE_2D_GAMMA)};

    PostRender __debug;
    Model cyborg{"../resources/cyborg/cyborg.obj"};


    virtual void application()
    {
        glEnable(GL_DEPTH_TEST);
        GLuint va;
        glGenVertexArrays(1, &va);
    }
    virtual void render_loop()
    {
        __debug.render_texture(_wood_texture);
    }
};

int main()
{
    DebugWidget widget(800, 600, "model");
    // MoedlWidget widget(800, 600, "model");
    widget.render();
    return 0;
}