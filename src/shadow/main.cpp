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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#include "GLWidget.hpp"

class shadowWidget : public GLWidget
{
    int _width;
    int _heigth;
    ShaderProgram shader;
    ShaderProgram depth_shader;
    unsigned int depthMapFBO;
    glm::vec3 lightPos{-2.0f, 4.0f, -1.0f};
    ShadowScene scene;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    unsigned int woodTexture = loadTexture("../resources/textures/wood.png");
    unsigned int depthMap;
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        unsigned int woodTexture = loadTexture("../resources/textures/wood.png");

        // configure depth map FBO
        // -----------------------
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH*2, SHADOW_HEIGHT*2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shader.load_vs_file("../glsl/shadow/shadow.vs");
        shader.load_fs_file("../glsl/shadow/shadow.fs");
        shader.link();

        depth_shader.load_vs_file("../glsl/shadow/depth.vs");
        depth_shader.load_fs_file("../glsl/shadow/depth.fs");
        depth_shader.link();

        depth_shader.use();
        depth_shader.set_uniform<int>("depthMap", 0);

        shader.use();
        shader.set_uniform<int>("diffuseTexture", 0);
        shader.set_uniform<int>("shadowMap", 1);
    }
    virtual void render_loop() override
    {
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, 1024*2, 1024*2);
        depth_shader.use();
        depth_shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glCullFace(GL_FRONT);//改变面剔除以解决阴影悬浮问题
        renderScene(depth_shader);
        glCullFace(GL_BACK); //不要忘记设回原先的面剔除
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        // glm::mat4 projection = glm::perspective(glm::radians(CAMERA.get_zoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(CAMERA.get_zoom()), (float)_width / (float)_heigth, 0.1f, 100.0f);
        glm::mat4 view = CAMERA.get_view_matrix();
        shader.set_uniform<glm::mat4>("projection", projection);
        shader.set_uniform<glm::mat4>("view", view);
        // set light uniforms
        shader.set_uniform<glm::vec3>("viewPos", CAMERA.get_position());
        shader.set_uniform<glm::vec3>("lightPos", lightPos);
        shader.set_uniform<glm::mat4>("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        scene.render(shader);
    }
public:
    shadowWidget(int width, int height, std::string_view title) : GLWidget(width,height,title), _width(width), _heigth(height)
    {

    }
};

int main()
{
    std::string title{"shadow"};
    shadowWidget widget(800,600,title);
    widget.render();
}