#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
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
    ShaderProgram shader{"../glsl/shadow/shadow.vs" ,"../glsl/shadow/shadow.fs"};
    ShaderProgram depth_shader{"../glsl/shadow/depth.vs" ,"../glsl/shadow/depth.fs"};
    glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};
    ShadowScene scene;
    glm::mat4 model = glm::mat4(1.0f);
    FrameBuffer fb{1024,1024};
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    unsigned int woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    virtual void application() override
    {

#ifdef __APPLE__//times 2 for apple retina
        _width *= 2;
        _height *= 2;
#endif
        glEnable(GL_DEPTH_TEST);

        fb.create_depth_attachment();

        depth_shader.use();
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        depth_shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);

        shader.use();
        shader.add_sampler("diffuseTexture", woodTexture);
        shader.add_sampler("shadowMap", fb.get_depth_texture());
        shader.set_uniform<glm::vec3>("lightPos", lightPos);
        shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);

    }

    virtual void render_loop() override
    {
        fb.update_viewport();
        glClear(GL_DEPTH_BUFFER_BIT);
        fb.bind();
        depth_shader.use();
        glCullFace(GL_FRONT);//改变面剔除以解决阴影悬浮问题
        scene.render(depth_shader);//pass1 渲染到fbo上
        glCullFace(GL_BACK); //不要忘记设回原先的面剔除
        fb.unbind();

        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.set_uniform<glm::mat4>("projection", get_projection());
        shader.set_uniform<glm::mat4>("view", CAMERA.get_view_matrix());
        shader.set_uniform<glm::vec3>("viewPos", CAMERA.get_position());
        shader.active_samplers();
        scene.render(shader);//pass2 实际渲染到屏幕上
    }

};

int main()
{
    std::string title{"shadow"};
    shadowWidget widget(800,600,title);
    widget.render();
}