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
    GLuint _result = 0;
    FrameBuffer _fb;
    Cube _cube;
public:
    virtual void execute(GLuint input = 0) = 0;
    operator GLuint()
    {
        return _result;
    }
};

class HdrToCubeRender : public PrecomputedRender
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    std::vector<glm::mat4> captureViews
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    ShaderProgram _sp{"../glsl/ibl/cube.vs", "../glsl/ibl/cube.fs"};
public:
    virtual void execute(GLuint hdr) override
    {
        _result = TEXTURE_MANAGER.generate_cube_texture_buffer(512, 512, TEXTURE_CUBE_RGB);
        _sp.use();
        _sp.set_uniform("equirectangularMap", 0);
        _sp.set_uniform("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdr);
        glViewport(0, 0, 512, 512);
        _fb.bind();
        for (unsigned int i = 0; i < 6; ++i)
        {
            _sp.set_uniform("view", captureViews[i]);
            _fb.attach_color_texture(0, _result, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _cube.render();
        }
        _fb.unbind();
    }
};

class IBL : public PrecomputedRender
{

};