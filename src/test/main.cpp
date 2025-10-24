#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.h"
#include "VertexArray.hpp"
#include <OpenGL/gltypes.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

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
    vVertexArray vao;

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


class Mesh
{
    vVertexArray _va;
    GLsizei _ct{ 0 };
    std::vector<float> vertices;
    std::vector<GLuint> indices;

    void set_up_va(const aiScene* scene, const aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices.insert(vertices.end(), 
            {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            });
            if (mesh->mTextureCoords[0]) 
            {
                vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
            }
            else 
            {
                vertices.insert(vertices.end(), {0, 0});
            }
        }
        auto vb_id = BUFFER.generate_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data());
        _va.attach_vertex_buffer(PNT_LAYOUT, vb_id);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
        {
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) 
            {
                indices.push_back(static_cast<GLuint>(mesh->mFaces[i].mIndices[j]));
            }
        }
        GLuint eb_id = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data());
        _va.attach_element_buffer(eb_id);
        _ct = static_cast<GLsizei>(indices.size());
    }

public:
    Mesh(const aiScene* scene, const aiMesh* mesh, std::string_view directory)
    {
        set_up_va(scene, mesh);
        // if (mesh->mMaterialIndex >= 0)
        // {
        //     TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], _textures, directory);
        // }
    }
    Mesh()
    {
        auto cube_vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(only_vertices_cube), only_vertices_cube);
        BufferLayout cube_lay;
        cube_lay.add_attribute(GL_FLOAT, 3);
        _va.attach_vertex_buffer(cube_lay, cube_vb);
    }
    void add_extra_data(BufferLayout layout, GLuint vb)
    {
        _va.attach_vertex_buffer(layout, vb);
    }
    void render_arrays(const ShaderProgram& sp)
    {
        _va.bind();
        sp.use();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        _va.unbind();
    }
    void render_elements(const ShaderProgram& sp)
    {
        sp.use();
        _va.bind();
        glDrawElements(GL_TRIANGLES, _ct, GL_UNSIGNED_INT, 0);
        _va.unbind();
    }
};

class CubeWidget : public GLWidget
{
    vVertexArray _va_tri;
    ShaderProgram _sp_tri{"../glsl/test/test.vs", "../glsl/test/test.fs"};

    vVertexArray _va_cube;
    ShaderProgram _sp_cube{"../glsl/test/cube.vs", "../glsl/test/cube.fs"};


    Mesh m;

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
        glm::mat4 model(1.0);
        _sp_cube.set_uniform("model", model);
        _sp_cube.set_uniform("view", CAMERA.get_view_matrix());
        _sp_cube.set_uniform("projection", get_projection());
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        m.render_arrays(_sp_cube);
        
        // _sp_cube.set_uniform("projection", get_projection());

    }
public:
    CubeWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) {}
};

int main()
{
    CubeWidget wid{800, 600, "test"};
    // ModelWidget wid{800, 600, "test"};
    // TWidget wid{800, 600, "test"};
    wid.render();
    return 0;
}