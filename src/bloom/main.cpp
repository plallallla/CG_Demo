#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "TextureAttributes.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "QuadRender.hpp"
#include <glm/detail/qualifier.hpp>
#include <string>
#include <xlocale/_stdio.h>

class BloomWidget : public GLWidget
{
public:
    BloomWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
        lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
        lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
        lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
        lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
        lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
        lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
        lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));
    }
private:
    ShaderProgram _shader{"../glsl/bloom/bloom.vs", "../glsl/bloom/bloom.fs"};
    ShaderProgram _light{"../glsl/bloom/bloom.vs", "../glsl/bloom/light.fs"};
    GLuint _cotainer_texture{TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_GAMMA_ALPHA)};
    GLuint _wood_texture{TEXTURE_MANAGER.load_texture("../resources/textures/wood.png", TEXTURE_2D_GAMMA)};
    FrameBuffer _fb_hdr;
    FrameBuffer _fb_pingpong_h;
    FrameBuffer _fb_pingpong_v;
    VertexArray _cube_va;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    QuadRender _debug;
    QuadRender _blur{"../glsl/bloom/blur.fs"};
    QuadRender _mixture{"../glsl/bloom/mixture.fs"};

    GLuint buffer_0{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};
    GLuint buffer_1{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};
    GLuint buffer_h{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};
    GLuint buffer_v{TEXTURE_MANAGER.generate_texture_buffer(800*2, 600*2, TEXTURE_2D_HDR)};

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
        _fb_hdr.create_render_object(_width * 2, _height * 2);
        _fb_hdr.active_draw_buffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});
        _fb_pingpong_h.bind();
        _fb_pingpong_h.attach_color_texture(0, buffer_h);
        _fb_pingpong_v.bind();
        _fb_pingpong_v.attach_color_texture(0, buffer_v);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _mixture._sp.use();
        _mixture._sp.set_sampler(0, "scene");
        _mixture._sp.set_sampler(1, "bloomBlur");

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

        glm::mat4 model;
       // create one large cube that acts as the floor
        _shader.active_sampler(0, _wood_texture);
        model = glm::mat4(1.0f);
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

        _light.use();
        _light.set_uniform("projection", get_projection());
        _light.set_uniform("view", CAMERA.get_view_matrix());
        for (int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lightPositions[i]));
            model = glm::scale(model, glm::vec3(0.25f));
            _light.set_uniform("model", model);
            _light.set_uniform("lightColor", lightColors[i]);
            render_cube();
        }
    }

    virtual void render_loop() override
    {
        _fb_hdr.bind();
        scene_render();

        _fb_pingpong_h.bind();
        _blur.render_texture(buffer_1);

        int amount = 50;
        for (int i = 1; i < amount; i++)
        {
            _blur._sp.set_uniform<int>("horizontal", int(i%2));
            if (i % 2)
            {
                _fb_pingpong_v.bind();
                _blur.render_texture(buffer_h);
            }
            else 
            {
                _fb_pingpong_h.bind();
                _blur.render_texture(buffer_v);           
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _mixture.render_texture({buffer_0, buffer_v});

    }
};

int main()
{
    BloomWidget w{800,600,"bloom"};
    w.render();
    return 0;
}