#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "ShadowScene.hpp"
#include "GLWidget.hpp"
#include "Texture.hpp"
#include "Frame.hpp"
#include "GLWidget.hpp"

class shadowWidget : public GLWidget
{
public:
    shadowWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    ShaderProgram shader;
    ShaderProgram depth_shader;
    glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};
    ShadowScene scene;
    glm::mat4 model = glm::mat4(1.0f);
    FrameBuffer fb{1024,1024};
    unsigned int _depth_texture;
    unsigned int _fbo;
    unsigned int woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    virtual void application() override
    {

#ifdef __APPLE__//times 2 for apple retina
        _width *= 2;
        _height *= 2;
#endif

        glEnable(GL_DEPTH_TEST);

        fb.create_depth_attachment();

        shader.load_vs_file("../glsl/shadow/shadow.vs");
        shader.load_fs_file("../glsl/shadow/shadow.fs");
        shader.link();

        shader.use();
        shader.add_sampler("diffuseTexture", 0);
        shader.add_sampler("shadowMap", 1);

        depth_shader.load_vs_file("../glsl/shadow/depth.vs");
        depth_shader.load_fs_file("../glsl/shadow/depth.fs");
        depth_shader.link();

    }

    virtual void render_loop() override
    {
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        depth_shader.use();
        depth_shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, 1024*2, 1024*2);

        glClear(GL_DEPTH_BUFFER_BIT);

        fb.bind();
        glCullFace(GL_FRONT);//改变面剔除以解决阴影悬浮问题
        scene.render(depth_shader);
        glCullFace(GL_BACK); //不要忘记设回原先的面剔除
        fb.unbind();

        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(CAMERA.get_zoom()), (float)_width / (float)_height, 0.1f, 100.0f);
        glm::mat4 view = CAMERA.get_view_matrix();
        shader.set_uniform<glm::mat4>("projection", projection);
        shader.set_uniform<glm::mat4>("view", view);
        shader.set_uniform<glm::vec3>("viewPos", CAMERA.get_position());
        shader.set_uniform<glm::vec3>("lightPos", lightPos);
        shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fb.get_depth_texture());

        scene.render(shader);
    }

};

int main()
{
    std::string title{"shadow"};
    shadowWidget widget(800,600,title);
    widget.render();
}