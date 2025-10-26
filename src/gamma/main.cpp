#include "Buffer.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

float planeVertices[] = {
    // positions            // normals         // texcoords
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

class MWidget : public GLWidget
{
    VertexArray _va;
    ShaderProgram _sp{"../glsl/gamma/gamma.vs", "../glsl/gamma/gamma.fs"};

    virtual void application() override
    {
        _va.attach_vertex_buffer
        (
            PNT_LAYOUT, 
            BUFFER.generate_buffer(GL_VERTEX_ARRAY, sizeof(planeVertices), planeVertices)
        );
        GLuint woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    }

    virtual void render_loop() override
    {

    }

public:
    MWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};

int main()
{

}