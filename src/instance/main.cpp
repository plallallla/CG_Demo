#include "Buffer.hpp"
#include "Camera.hpp"
#include "GLWidget.hpp"
#include "Model.hpp"
#include "VertexArray.hpp"
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>

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
        auto quad_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices);
        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 2);
        layout.add_attribute(GL_FLOAT, 3);
        _quad_va.attach_vertex_buffer(layout, quad_vb);

        auto index_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations);
        BufferLayout index_layout;
        index_layout.add_attribute(GL_FLOAT, 2, true, true, 1);

        _quad_va.attach_vertex_buffer(index_layout, index_vb);
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
    unsigned int amount = 100000;
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

        auto mm_id = BUFFER.generate_buffer(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0]);

        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 4, true, true, 1);
        layout.add_attribute(GL_FLOAT, 4, true, true, 1);
        layout.add_attribute(GL_FLOAT, 4, true, true, 1);
        layout.add_attribute(GL_FLOAT, 4, true, true, 1);

        _rock.attach_extra_buffer(layout, mm_id);
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
        _planet.render_elements(_shader);

        // 绘制小行星 使用uniform来设置model
        // for(unsigned int i = 0; i < amount; i++)
        // {
        //     _shader.set_uniform("model", modelMatrices[i]);
        //     _rock.render_elements(_shader);
        // }

        // 绘制小行星 使用instance来设置model
        _instance_shader.use();
        _instance_shader.set_uniform("view", CAMERA.get_view_matrix());
        _rock.render_elements_instanced(_instance_shader, amount);

    }
};

int main()
{
    // InstanceWidget w{ 800, 600, "instance" };
    UniverseWidget w{ 800, 600, "universe" };
    w.render();
    return 0;
}