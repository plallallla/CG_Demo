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
    ShaderProgram shader{"../glsl/shadow/point_shadows.vs" ,"../glsl/shadow/point_shadows.fs"};
    ShaderProgram depth_shader{"../glsl/shadow/point_shadows_depth.vs", "../glsl/shadow/point_shadows_depth.gs" ,"../glsl/shadow/point_shadows_depth.fs"};    
    // GLuint depth_texture = TEXTURE_MANAGER.generate_cube_texture_buffer(1024, 1024);
    // FrameBuffer frame;
    glm::vec3 lightPos{0.0f, 0.5f, 0.0f};
    ShadowScene scene;

    unsigned int woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    unsigned int depthCubemap;

    virtual void application() override
    {
#ifdef __APPLE__//times 2 for apple retina
        _width *= 2;
        _height *= 2;
#endif        
        // glEnable(GL_DEPTH_TEST);
        // frame.bind();
        // frame.attach_depth_texture_array(depth_texture);
        // frame.set_draw_read(GL_NONE, GL_NONE);
        // frame.unbind();
        shader.use();
        shader.set_sampler(0, "diffuseTexture");
        shader.set_sampler(1, "depthMap");


    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    // configure depth map FBO
    // -----------------------

        glGenFramebuffers(1, &depthMapFBO);
        // create depth cubemap texture
        glGenTextures(1, &depthCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH*2, SHADOW_HEIGHT*2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
   
    }

    virtual void render_loop() override
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH*2, SHADOW_HEIGHT*2);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depth_shader.use();
        for (unsigned int i = 0; i < 6; ++i)
            depth_shader.set_uniform("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        depth_shader.set_uniform("far_plane", far_plane);
        depth_shader.set_uniform("lightPos", lightPos);
        scene.render(depth_shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(CAMERA.get_zoom()), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 0.1f, 100.0f);
        shader.set_uniform("projection", get_projection());
        shader.set_uniform("view", CAMERA.get_view_matrix());
        // set lighting uniforms
        shader.set_uniform("lightPos", lightPos);
        shader.set_uniform("viewPos", CAMERA.get_position());
        shader.set_uniform("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        scene.render(shader);
    }

public:
    PointShadow(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {

    }
};

int main()
{
    // shadowWidget widget(800,600,"shadow");
    PointShadow widget(800,600,"shadow");
    widget.render();
    return 0;
}