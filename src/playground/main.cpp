#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "ShadowScene.hpp"
#include "Texture.hpp"

class CWidhet : public GLWidget
{
    ShadowScene _scene;
    ShaderProgram _sp{"../glsl/playground/trans.vs", "../glsl/playground/basic.fs"};
    GLuint _wood = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    GLuint spec_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2_specular.png", TEXTURE_2D_RGBA);
    GLuint diff_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_RGBA);
    virtual void application() override
    {
        _sp.set_sampler(0, "diff");
        _sp.use();
    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("projection", get_projection());
        _scene.render(_sp, _wood, diff_texture);
    }

public:
    CWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {

    }
};

int main()
{
    CWidhet c{800, 600, "1111"};
    c.render();
    return 0;
}