#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "QuadRender.hpp"
#include <string>
#include <xlocale/_stdio.h>

std::vector<glm::vec3> lightPositions
{
   glm::vec3( 0.0f, 0.5f,  1.5f),
   glm::vec3(-4.0f, 0.5f, -3.0f),
   glm::vec3( 3.0f, 0.5f,  1.0f),
   glm::vec3(-.8f,  2.4f, -1.0f),
};
std::vector<glm::vec3> lightColors
{
    glm::vec3(5.0f, 5.0f, 5.0f),
    glm::vec3(10.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 15.0f),
    glm::vec3(0.0f, 5.0f, 0.0f),
};

class BloomWidget : public GLWidget
{
public:
    BloomWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
private:
    ShaderProgram _shader{"../glsl/bloom/bloom.vs", "../glsl/bloom/bloom.fs"};
    GLuint _cotainer_texture{TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_GAMMA_ALPHA)};
    GLuint _wood_texture{TEXTURE_MANAGER.load_texture("../resources/textures/wood.png", TEXTURE_2D_GAMMA)};
    FrameBuffer _fb_hdr;
    FrameBuffer _fb_pingpong;
    VertexArray _cube_va;

    QuadRender _debug;

    GLuint buffer_0{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};
    GLuint buffer_1{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};

    void render_cube()
    {
        _cube_va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    virtual void application() override
    {

        glEnable(GL_DEPTH_TEST);

        float vertices[] = 
        {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };


        _cube_va.attach_vertex_buffer(PNT_LAYOUT, BUFFER.generate_vertex_buffer(sizeof(vertices), vertices));

        _shader.use();
        _shader.set_sampler(0, "diffuseTexture");

        _fb_hdr.bind();
        _fb_hdr.attach_color_texture(0, buffer_0);
        _fb_hdr.attach_color_texture(1, buffer_1);
        _fb_hdr.create_render_object(_width*2, _height*2);
        _fb_hdr.unbind();

        // _fb_pingpong.bind();
        // _fb_pingpong.attach_color_texture(TEXTURE_MANAGER.generate_texture_buffer(800, 600, TEXTURE_2D_HDR));
        // _fb_pingpong.attach_color_texture(TEXTURE_MANAGER.generate_texture_buffer(800, 600, TEXTURE_2D_HDR));
    }

    void scene_render()
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _cube_va.bind();
        _shader.use();
        for (int i = 0; i < 4; i++)
        {
            _shader.set_uniform("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            _shader.set_uniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        }
        _shader.set_uniform("projection", get_projection());
        _shader.set_uniform("view", CAMERA.get_view_matrix());
        _shader.active_sampler(0, _wood_texture);
        _shader.set_uniform("viewPos", CAMERA.get_position());

        // create one large cube that acts as the floor
        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
        model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
        _shader.set_uniform("model", model);
        render_cube();

        // then create multiple cubes as the scenery
        _shader.active_sampler(0, _cotainer_texture);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _shader.set_uniform("model", model);
        _shader.set_uniform("model", model);
        render_cube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _shader.set_uniform("model", model);
        render_cube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        _shader.set_uniform("model", model);
        render_cube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
        model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(1.25));
        _shader.set_uniform("model", model);
        render_cube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
        model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        _shader.set_uniform("model", model);
        render_cube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _shader.set_uniform("model", model);
        render_cube();
    }

    virtual void render_loop() override
    {
        _fb_hdr.bind();
        scene_render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _debug.render_texture(buffer_1);

    }
};

class TWidget : public GLWidget
{
    ShaderProgram s{"../glsl/bloom/test.vs", "../glsl/bloom/test.fs"};
    VertexArray vao;

    virtual void application() override
    {
        float vertices_with_color[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};
        GLuint VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices_with_color), vertices_with_color);
        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 3);
        layout.add_attribute(GL_FLOAT, 3);
        vao.attach_vertex_buffer(layout, VBO);
    }

    virtual void render_loop() override
    {
        s.use();
        vao.bind();
        // s.set_uniform("lights[0].Position", lightPositions[0]);
        // s.set_uniform("lights[0].Color", lightColors[0]);
        // s.set_uniform("lights[1].Position", lightPositions[1]);
        // s.set_uniform("lights[1].Color", lightColors[1]);



        s.set_uniform("lights[0].Position", lightPositions[0]);
        s.set_uniform("lights[0].Color", lightColors[0]);
        s.set_uniform("lights[1].Position", lightPositions[1]);
        s.set_uniform("lights[1].Color", lightColors[1]);
        s.set_uniform("lights[2].Position", lightPositions[2]);
        s.set_uniform("lights[2].Color", lightColors[2]);
        s.set_uniform("lights[3].Position", lightPositions[3]);
        s.set_uniform("lights[3].Color", lightColors[3]);


        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(0);
    }
public:
    TWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};


int main()
{
    // TWidget w{800,600,"bloom"};
    BloomWidget w{800,600,"bloom"};
    w.render();
    return 0;
}