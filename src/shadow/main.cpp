#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "ShaderProgram.hpp"
#include "ShadowScene.hpp"
#include "GLWidget.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "GLWidget.hpp"
#include "TextureAttributes.hpp"
#include "QuadRender.hpp"

class shadowWidget : public GLWidget
{
public:
    shadowWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    ShaderProgram shader{"../glsl/shadow/shadow.vs" ,"../glsl/shadow/shadow.fs"};
    ShaderProgram depth_shader{"../glsl/shadow/depth.vs" ,"../glsl/shadow/depth.fs"};
    glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};
    ShadowScene scene;
    glm::mat4 model = glm::mat4(1.0f);
    FrameBuffer fb;
    GLuint depth_texture = TEXTURE_MANAGER.generate_texture_buffer(1024*2, 1024*2, TEXTURE_2D_DEPTH);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    unsigned int woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    QuadRender debug{"../glsl/shadow/quad.fs"};
    virtual void application() override
    {

#ifdef __APPLE__//times 2 for apple retina
        _width *= 2;
        _height *= 2;
#endif
        glEnable(GL_DEPTH_TEST);

        CAMERA.set_position({0,0.5,5.});

        fb.bind();
        fb.attach_depth_texture(depth_texture);

        depth_shader.use();
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        depth_shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);

        shader.use();
        shader.set_sampler(0, "diffuseTexture");
        shader.set_sampler(1, "shadowMap");
        shader.set_uniform<glm::vec3>("lightPos", lightPos);
        shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);

    }

    virtual void render_loop() override
    {
        glViewport(0, 0, 1024*2, 1024*2);
        glClear(GL_DEPTH_BUFFER_BIT);
        fb.bind();
        depth_shader.use();
        glCullFace(GL_FRONT);//改变面剔除以解决阴影悬浮问题
        scene.render(depth_shader);//pass1 渲染到fbo上
        glCullFace(GL_BACK); //不要忘记设回原先的面剔除
        fb.unbind();

        glViewport(0, 0, _width, _height);

        // debug.render_texture(depth_texture);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.set_uniform<glm::mat4>("projection", get_projection());
        shader.set_uniform<glm::mat4>("view", CAMERA.get_view_matrix());
        shader.set_uniform<glm::vec3>("viewPos", CAMERA.get_position());
        shader.active_sampler(0, woodTexture);
        shader.active_sampler(1, depth_texture);
        scene.render(shader);//pass2 实际渲染到屏幕上
    }

};

class PointShadow : public GLWidget
{
    virtual void application() override
    {
    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

public:
    PointShadow(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {

    }
};

int main()
{
    shadowWidget widget(800,600,"shadow");
    widget.render();
    return 0;
}