#pragma once
#include "FrameBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Shape.hpp"
#include "Texture.hpp"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class PrecomputedRender
{
protected:

    inline static const std::vector<glm::mat4> capture_views = []() -> auto
    {
        return std::vector<glm::mat4>
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
    }();

    inline static const glm::mat4 capture_projection = [] () -> auto
    {
        return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    }();

    GLuint _result = 0;
    FrameBuffer _fb;
    Cube _cube_shape;
    GLuint _width;
    GLuint _height;
public:
    PrecomputedRender(GLuint width = 512, GLuint height = 512) : _width{ width }, _height{ height } {}
    virtual void execute(GLuint input = 0) = 0;
    operator GLuint()
    {
        return _result;
    }
};

class HdrCubeRender : public PrecomputedRender
{
    ShaderProgram _sp{"../glsl/ibl/cube.vs", "../glsl/ibl/cube.fs"};
public:
    HdrCubeRender(GLuint width = 512, GLuint height = 512) : PrecomputedRender{ width, height } {}
    virtual void execute(GLuint hdr_texture) override
    {
        _result = TEXTURE_MANAGER.generate_cube_texture_buffer(_width, _height, TEXTURE_CUBE_RGB);
        glViewport(0, 0, _width, _height);
        _sp.use();
        _sp.set_uniform("equirectangularMap", 0);
        _sp.set_uniform("projection", capture_projection);
        _sp.active_sampler(0, hdr_texture);
        _fb.bind();
        for (unsigned int i = 0; i < 6; ++i)
        {
            _sp.set_uniform("view", capture_views[i]);
            _fb.attach_color_texture(0, _result, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _cube_shape.render();
        }
        _fb.unbind();
    }
};

class DiffuseIrradianceIBL : public PrecomputedRender
{
    ShaderProgram _sp{"../glsl/ibl/cube.vs", "../glsl/ibl/irradiance_convolution.fs"};
public:
    DiffuseIrradianceIBL(GLuint width = 64, GLuint height = 64) : PrecomputedRender{ width, height } {}
    virtual void execute(GLuint cube_texture) override
    {
        _result = TEXTURE_MANAGER.generate_cube_texture_buffer(32, 32, TEXTURE_CUBE_RGB_FLOAT);
        glViewport(0, 0, 32, 32);
        _fb.bind();
        _fb.create_render_object(32, 32);
        _sp.use();
        _sp.set_sampler(0, "environmentMap");
        _sp.set_uniform("projection", capture_projection);
        _sp.active_sampler(0, cube_texture);
        for (unsigned int i = 0; i < 6; ++i)
        {
            _sp.set_uniform("view", capture_views[i]);
            _fb.attach_color_texture(0, _result, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _cube_shape.render();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};