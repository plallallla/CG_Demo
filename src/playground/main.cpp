#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

class CWidhet : public GLWidget
{
    // ShaderProgram _sp{"../glsl/playground/trans.vs", "../glsl/playground/basic.fs"};
    ShaderProgram _sp{"../glsl/playground/trans.vs", "../glsl/playground/bp.fs"};
    GLuint _wood = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    GLuint spec_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2_specular.png", TEXTURE_2D_RGBA);
    GLuint diff_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_RGBA);
    VertexArray plane_va;
    VertexArray cube_va;
    void renderPlane()
    {
        plane_va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    void renderCube()
    {
        cube_va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    virtual void application() override
    {
        _sp.use();
        _sp.set_sampler(0, "material.ambient");
        _sp.set_sampler(1, "material.diffuse");
        _sp.set_sampler(2, "material.specular");

        _sp.set_uniform("material.shininess", 128);

        _sp.set_uniform("light.position", glm::vec3(0.0, 1.0, 3.0));
        _sp.set_uniform("light.color", glm::vec3(1.0, 1.0, 1.0));

    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("view_pos", CAMERA.get_position());
        _sp.set_uniform("projection", get_projection());

        glm::mat4 model = glm::mat4(1.0f);
        // floor
        _sp.active_sampler(0, _wood);
        _sp.active_sampler(1, _wood);
        _sp.active_sampler(2, 0);        
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderPlane();
        // cubes
        _sp.active_sampler(0, diff_texture);
        _sp.active_sampler(1, diff_texture);
        _sp.active_sampler(2, spec_texture);  
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.25));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
    }

public:
    CWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        {
            float planeVertices[] = 
            {
            // positions            // normals         // texcoords
            25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, 25.0f,  0.0f,
            1.0f,  0.0f,  0.0f,  0.0f,  -25.0f, -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  0.0f,   25.0f,

            25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, -25.0f, 0.0f,
            1.0f,  0.0f,  0.0f,  25.0f, 25.0f,  -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  25.0f,  25.0f
            };
            GLuint vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices);
            plane_va.attach_vertex_buffer(PNT_LAYOUT, vb);
        }
        {
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
            GLuint vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
            cube_va.attach_vertex_buffer(PNT_LAYOUT, vb);
        }
    }
};

int main()
{
    CWidhet c{800, 600, "1111"};
    c.render();
    return 0;
}