#include "Camera.hpp"
#include "GLWidget.hpp"
#include "Model.hpp"
#include "ShaderProgram.h"
#include "Vertex.h"
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <map>

class InstanceWidget : public GLWidget
{
public:
    InstanceWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        int index = 0;
        float offset = 0.1f;
        for(int y = -10; y < 10; y += 2)
        {
            for(int x = -10; x < 10; x += 2)
            {
                glm::vec2 translation;
                translation.x = (float)x / 10.0f + offset;
                translation.y = (float)y / 10.0f + offset;
                translations[index++] = translation;
            }
        }
    }
private:
    ShaderProgram _quad{"../glsl/instance/quad.vs", "../glsl/instance/quad.fs"};

    VertexArray _quad_va;
    glm::vec2 translations[100];
    unsigned int quadVAO, quadVBO;
    virtual void application() override
    {
        float quadVertices[] = 
        {
            // 位置          // 颜色
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
        
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
            0.05f,  0.05f,  0.0f, 1.0f, 1.0f                   
        };
        VertexBuffer quad_vb;
        quad_vb.set_data(sizeof(quadVertices), quadVertices);
        quad_vb.add_layout(GL_FLOAT, 2, true);
        quad_vb.add_layout(GL_FLOAT, 3, true);
        _quad_va.addVertexBuffer(quad_vb);
        VertexBuffer index_vb;
        index_vb.set_data(sizeof(glm::vec2) * 100, &translations);
        index_vb.add_layout(GL_FLOAT, 2, true);
        _quad_va.addVertexBuffer(index_vb);
        _quad_va.setAttributeDivisor(2, 1);
    }
    virtual void render_loop() override
    {
        _quad.use();
        _quad_va.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    }
};

void generate_model_matrices(std::vector<glm::mat4>& modelMatrices, size_t amount)
{
    modelMatrices = std::vector<glm::mat4>(amount);
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));
        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));
        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }
}

class UniverseWidget : public GLWidget
{
public:
    UniverseWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        generate_model_matrices(modelMatrices, amount);
    }
private:
    ShaderProgram _shader{"../glsl/instance/universe.vs", "../glsl/instance/universe.fs"};
    ShaderProgram _instance_shader{"../glsl/instance/universe_instance.vs", "../glsl/instance/universe.fs"};
    unsigned int amount = 10000;
    std::vector<glm::mat4> modelMatrices;

    Model _planet{"../resources/planet/planet.obj"};
    Model _rock{"../resources/rock/rock.obj"};

    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        CAMERA.set_position({0.0f, 0.0f, 55.0f});
        _shader.use();
        _shader.set_uniform("projection", get_projection());
        _instance_shader.use();
        _instance_shader.set_uniform("projection", get_projection());

        VertexBuffer matrices_buffer;
        matrices_buffer.set_data(amount * sizeof(glm::mat4), modelMatrices.data());
        matrices_buffer.add_layout(GL_FLOAT, 4, true);
        matrices_buffer.add_layout(GL_FLOAT, 4, true);
        matrices_buffer.add_layout(GL_FLOAT, 4, true);
        matrices_buffer.add_layout(GL_FLOAT, 4, true);
        _rock.add_vertexbuffer(matrices_buffer);
        _rock.sets_attributes_divisor({{3, 1},{4, 1},{5, 1},{6, 1}});
    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _shader.use();
        _shader.set_uniform("view", CAMERA.get_view_matrix());

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        _shader.set_uniform("model", model);
        _planet.Draw(_shader);

        // glUseProgram(0);

        // _instance_shader.use();
        // _instance_shader.set_uniform("view", CAMERA.get_view_matrix());

        // draw meteorites
        // for (unsigned int i = 0; i < amount; i++)
        // {
            // _rock.draw_instance(_instance_shader, i);
        // }

    }
};

int main()
{
    InstanceWidget w{ 800, 600, "instance" };
    // UniverseWidget w{ 800, 600, "universe" };
    w.render();
    return 0;
}