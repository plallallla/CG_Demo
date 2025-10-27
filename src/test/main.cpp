#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "VertexArray.hpp"
#include <OpenGL/gltypes.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <cstddef>

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };


    float vertices_with_color[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

    float only_vertices[] = {
    // 位置            
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

    float only_color[] = {// 颜色
    1.0f, 0.0f, 0.0f,   // 右下
    0.0f, 1.0f, 0.0f,   // 左下
    0.0f, 0.0f, 1.0f    // 顶部
};

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

        unsigned int indices_1[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
    };
        unsigned int indices_2[] = {  // note that we start from 0!
        1, 2, 3   // second Triangle
    };


#include "Buffer.hpp"
#include "VertexArray.hpp"

class TWidget : public GLWidget
{
    ShaderProgram s{"../glsl/test/test.vs", "../glsl/test/test.fs"};
    VertexArray vao;

    virtual void application() override
    {
        GLuint VBO = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices_with_color), vertices_with_color);
        // GLuint eb1 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1);
        // GLuint eb2 = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2);
        BufferLayout layout;
        layout.add_attribute(GL_FLOAT, 3);
        layout.add_attribute(GL_FLOAT, 3);
        // GLuint ver = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices), only_vertices);
        // GLuint col = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_color), only_color);
        // vao.attach_vertex_buffer(layout, ver);
        // vao.attach_vertex_buffer(layout, col);
        vao.attach_vertex_buffer(layout, VBO);
    }

    virtual void render_loop() override
    {
        s.use();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(0);
    }
public:
    TWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}

};


// #include "Model.hpp"

float cube[] = {
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

    float only_vertices_cube[] = {
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f, 
    };



#include "Model.hpp"



class CubeWidget : public GLWidget
{
    VertexArray _va_tri;
    ShaderProgram _sp_tri{"../glsl/test/test.vs", "../glsl/test/test.fs"};

    VertexArray _va_cube;
    ShaderProgram _sp_cube{"../glsl/test/cube.vs", "../glsl/test/cube.fs"};


    Model ourModel{"../resources/backpack/backpack.obj"};

    ShaderProgram _back_shader{"../glsl/test/backup.vs", "../glsl/test/backup.fs"};


    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);

        CAMERA.set_position({0,0,3});

        auto tri_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices_with_color), vertices_with_color);
        BufferLayout lay;
        lay.add_attribute(GL_FLOAT, 3);
        lay.add_attribute(GL_FLOAT, 3);
        _va_tri.attach_vertex_buffer(lay, tri_vb);

        auto cube_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices_cube), only_vertices_cube);
        BufferLayout cube_lay;
        cube_lay.add_attribute(GL_FLOAT, 3);
        _va_cube.attach_vertex_buffer(cube_lay, cube_vb);

        // auto cube_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices_cube), only_vertices_cube);
        // BufferLayout cube_lay;
        // cube_lay.add_attribute(GL_FLOAT, 3);
        // _va_cube.attach_vertex_buffer(cube_lay, cube_vb);

    }
    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // _va_tri.bind();
        // _sp_tri.use();
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // _va_cube.bind();
        // _sp_cube.use();
        // _sp_cube.set_uniform("model", model);
        // _sp_cube.set_uniform("view", CAMERA.get_view_matrix());
        // _sp_cube.set_uniform("projection", get_projection());
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // m.render_arrays(_sp_cube);
        // for (size_t i = 0; i < ourModel._meshes.size(); i++) 
        // {
        //     ourModel._meshes[i].render_elements(_sp_cube);
        // }

        
        // _sp_cube.set_uniform("projection", get_projection());

        _back_shader.use();
        glm::mat4 model(1.0);
        _back_shader.set_uniform("model", model);
        _back_shader.set_uniform("view", CAMERA.get_view_matrix());
        _back_shader.set_uniform("projection", get_projection());
        // ourModel.render_elements(_back_shader);

    }
public:
    CubeWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
};

// #include "glModel.hpp"

class ModelWidget : public GLWidget
{
    Model vourModel{"../resources/backpack/backpack.obj"};
    // Model cyborg{"../resources/cyborg/cyborg.obj"};
    ShaderProgram _back_shader{"../glsl/test/backup.vs", "../glsl/test/backup.fs"};

    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        CAMERA.set_position({0,0,3});
        unsigned int woodTexture = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    }
    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _back_shader.use();
        glm::mat4 model(1.0);
        _back_shader.set_uniform("model", model);
        _back_shader.set_uniform("view", CAMERA.get_view_matrix());
        _back_shader.set_uniform("projection", get_projection());

        // for (unsigned int i = 0; i < vourModel._meshes.size(); i++)
        // {
        //     auto& m = vourModel._meshes[i];
        //     int sampler_offset = _back_shader.get_samplers_ct();
        //     for (int i = 0; i < m._textures.size(); i++)
        //     {
        //         _back_shader.set_uniform<int>(m._textures[i].type, sampler_offset);
        //         glActiveTexture(GL_TEXTURE0 + sampler_offset);
        //         glBindTexture(GL_TEXTURE_2D, m._textures[i].id);
        //         sampler_offset++;
        //     }
        //     m._va.bind();
        //     glDrawElements(GL_TRIANGLES, m._elements_ct, GL_UNSIGNED_INT, 0);
        //     m._va.unbind();
        // }

        vourModel.render_elements(_back_shader);
        // cyborg.render_elements(_back_shader);

        glUseProgram(0);
    }
public:
    ModelWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
};


int main()
{
    ModelWidget wid{800, 600, "test"};
    // CubeWidget wid{800, 600, "test"};
    // TWidget wid{800, 600, "test"};
    wid.render();
    return 0;
}