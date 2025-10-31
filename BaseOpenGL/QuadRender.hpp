#include "ShaderProgram.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

class QuadRender
{
    ShaderProgram _sp;
    VertexArray _va;
    static std::string default_vs_src;
    static std::string default_fs_src;
    void set_up_va()
    {
        float quadVertices[] = 
        {
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        BufferLayout layout_22;
        layout_22.add_attribute(GL_FLOAT, 2);
        layout_22.add_attribute(GL_FLOAT, 2);
        _va.attach_vertex_buffer
        (
            layout_22, 
            BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices)
        );
    }
public:
    QuadRender(std::string_view fs_path)
    {
        _sp.load_vs_src(default_vs_src);
        _sp.load_fs_file(fs_path);
        _sp.link();
        set_up_va();
    }
    QuadRender()
    {
        _sp.load_vs_src(default_vs_src);
        _sp.load_fs_src(default_fs_src);
        _sp.link();
        set_up_va();
    }
    void render_texture(GLuint texture)
    {
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        _sp.use();
        _va.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

inline std::string QuadRender::default_vs_src = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
)";

inline std::string QuadRender::default_fs_src = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
void main()
{
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
} 
)";