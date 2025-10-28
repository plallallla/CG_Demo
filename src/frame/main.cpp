#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "TextureAttributes.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"

float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
float planeVertices[] = {
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};
float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};

class FrameWidget : public GLWidget
{
    VertexArray _va_cube;
    VertexArray _va_plane;
    VertexArray _va_quad;

    GLuint _cube_texture{TEXTURE_MANAGER.load_texture("../resources/textures/container.jpg")};
    GLuint _floor_texture{TEXTURE_MANAGER.load_texture("../resources/textures/metal.png")};

    ShaderProgram _sp{"../glsl/frame/framebuffers.vs", "../glsl/frame/framebuffers.fs"};
    ShaderProgram _screen{"../glsl/frame/framebuffers_screen.vs", "../glsl/frame/framebuffers_screen.fs"};
    ShaderProgram _triangle{"../glsl/triangle/triangle.vs", "../glsl/triangle/triangle.fs"};


    GLuint framebuffer;
    GLuint textureColorbuffer;


    virtual void application() override
    {
        BufferLayout layout_32;
        layout_32.add_attribute(GL_FLOAT, 3);
        layout_32.add_attribute(GL_FLOAT, 2);
        BufferLayout layout_22;
        layout_22.add_attribute(GL_FLOAT, 2);
        layout_22.add_attribute(GL_FLOAT, 2);
        _va_cube.attach_vertex_buffer
        (
            layout_32, 
            BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices)
        );
        _va_plane.attach_vertex_buffer
        (
            layout_32, 
            BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices)
        );
        _va_quad.attach_vertex_buffer
        (
            layout_22, 
            BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices)
        );
        _sp.add_sampler("texture1", 1);


        // framebuffer configuration
        // -------------------------
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a color attachment texture
        textureColorbuffer = TEXTURE_MANAGER.generate_texture_buffer(_width*2, _height*2, TEXTURE_2D_RGB);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width*2, _height*2); // use a single renderbuffer object for both a depth AND stencil buffer.
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) LOG.info("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void render_scene()
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _sp.use();
        glm::mat4 model;
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("projection", get_projection());
        // cubes
        _va_cube.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _cube_texture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        _sp.set_uniform("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        _sp.set_uniform("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // plane
        _va_plane.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _floor_texture);
        model = glm::mat4(1.0f);
        _sp.set_uniform("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void render_texture(GLuint texture)
    {
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        _screen.use();
        _va_quad.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    virtual void render_loop() override
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        render_scene();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        render_texture(textureColorbuffer);



        // render_scene();


    }
public:
    FrameWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};

int main()
{
    FrameWidget w{800,600,"frame"};
    w.render();
    return 0;
}