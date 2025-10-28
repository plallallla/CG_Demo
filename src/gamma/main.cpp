#include "Buffer.hpp"
#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "TextureAttributes.hpp"
#include "VertexArray.hpp"
#include <glm/fwd.hpp>

float planeVertices[] = {
    // positions            // normals         // texcoords
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

// lighting info
// -------------
glm::vec3 lightPositions[] = {
    glm::vec3(-3.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3 (1.0f, 0.0f, 0.0f),
    glm::vec3 (3.0f, 0.0f, 0.0f)
};
glm::vec3 lightColors[] = {
    glm::vec3(0.25),
    glm::vec3(0.50),
    glm::vec3(0.75),
    glm::vec3(1.00)
};

    float only_vertices[] = {
    // 位置            
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

class MWidget : public GLWidget
{
    VertexArray _va;
    ShaderProgram _sp{"../glsl/gamma/gamma.vs", "../glsl/gamma/gamma.fs"};
    ShaderProgram _triangle{"../glsl/triangle/triangle.vs", "../glsl/triangle/triangle.fs"};

    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glEnable(GL_FRAMEBUFFER_SRGB);

        CAMERA.set_position({0.0, 0.0, 3.0});

        _va.attach_vertex_buffer
        (
            PNT_LAYOUT, 
            BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices)
        );

        // auto ta = TEXTURE_2D_RGB;
        auto ta = TEXTURE_2D_GAMMA;
        GLuint woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png", ta);
        _sp.use();
        _sp.add_sampler("floorTexture", woodTexture);
        _sp.set_uniform_vector<glm::vec3>("lightPositions", 4, &lightPositions[0]);
        _sp.set_uniform_vector<glm::vec3>("lightColors", 4, &lightColors[0]);
    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _sp.use();
        _sp.active_samplers();
        _sp.set_uniform("projection", get_projection());
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("viewPos", CAMERA.get_position());
        _sp.set_uniform<int>("gamma", 1);
        _va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

public:
    MWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};

int main()
{
    MWidget w{800,600,""};
    w.render();
    return 0;
}