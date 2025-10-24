#pragma once
#include <OpenGL/gltypes.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <cstddef>
#include <string>
#include <vector>
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "ShaderProgram.h"

class Mesh
{
    vVertexArray _va;
    Textures _textures;
    GLsizei _ct{ 0 };
    void set_up_va(const aiScene* scene, const aiMesh* mesh)
    {
        std::vector<float> vertices;
        std::vector<GLuint> indices;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
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
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
        {
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) 
            {
                indices.push_back(static_cast<GLuint>(mesh->mFaces[i].mIndices[j]));
            }
        }
        GLuint vb_id = BUFFER.generate_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data());
        GLuint eb_id = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data());
        _va.attach_buffer(PNT_LAYOUT, vb_id, eb_id);
        _ct = static_cast<GLsizei>(indices.size());
    }

public:
    Mesh(const aiScene* scene, const aiMesh* mesh, std::string_view directory)
    {
        set_up_va(scene, mesh);
        if (mesh->mMaterialIndex >= 0)
        {
            TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], _textures, directory);
        }
    }
    void Draw(const ShaderProgram& shader)
    {
        shader.use();
        int sampler_offset = shader.get_samplers_ct();
        for (int i = 0; i < _textures.size(); i++)
        {
            shader.set_uniform<int>(_textures[i].type, sampler_offset);
            glActiveTexture(GL_TEXTURE0 + sampler_offset);
            glBindTexture(GL_TEXTURE_2D, _textures[i].id);
            sampler_offset++;
        }
        _va.bind();
        glDrawElements(GL_TRIANGLES, _ct, GL_UNSIGNED_INT, 0);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        _va.unbind();
    }
    void add_extra_data(BufferLayout layout, GLuint vb)
    {
        _va.attach_vertex_buffer(layout, vb);
    }
    void render_elements(const ShaderProgram& sp)
    {
        sp.use();
        int sampler_offset = sp.get_samplers_ct();
        for (int i = 0; i < _textures.size(); i++)
        {
            sp.set_uniform<int>(_textures[i].type, sampler_offset);
            glActiveTexture(GL_TEXTURE0 + sampler_offset);
            glBindTexture(GL_TEXTURE_2D, _textures[i].id);
            sampler_offset++;
        }
        _va.bind();
        glDrawElements(GL_TRIANGLES, _ct, GL_UNSIGNED_INT, 0);
        _va.unbind();
    }
};

class Model
{
    std::string _directory;
    void process_node(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            _meshes.emplace_back(scene, scene->mMeshes[node->mMeshes[i]], _directory);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            process_node(node->mChildren[i], scene);
        }
    }
public:
    std::vector<Mesh> _meshes;
    Model(std::string_view path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG.info("ERROR::ASSIMP::" + std::string{ importer.GetErrorString() });
            return;
        }
        _directory = path.substr(0, path.find_last_of('/'));
        process_node(scene->mRootNode, scene);
    }
    void render_elements(const ShaderProgram& sp)
    {
        for (size_t i = 0; i < _meshes.size(); i++)
        {
            std::cout << i << std::endl;
            _meshes[i].render_elements(sp);
        }
    }

    void Draw(const ShaderProgram& sp)
    {
        for (size_t i = 0; i < _meshes.size(); i++)
        {
            std::cout << i << std::endl;
            _meshes[i].render_elements(sp);
        }
    }
};

// class Model
// {
//   public:
//     std::vector<Mesh> _meshes;
//     Model(std::string path)
//     {
//         loadModel(path);
//     }
//     void Draw(ShaderProgram shader)
//     {
//         for (unsigned int i = 0; i < _meshes.size(); i++)
//         {
//             _meshes[i].Draw(shader);
//         }

//     }

//   private:
//     std::string _directory;
//     void loadModel(std::string path)
//     {
//         Assimp::Importer importer;
//         const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
//         if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//         {
//             // cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
//             return;
//         }
//         _directory = path.substr(0, path.find_last_of('/'));
//         processNode(scene->mRootNode, scene);
//     }
//     void processNode(aiNode* node, const aiScene* scene)
//     {
//         // 处理节点所有的网格（如果有的话）
//         for (unsigned int i = 0; i < node->mNumMeshes; i++)
//         {
//             aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//             _meshes.emplace_back(scene, scene->mMeshes[node->mMeshes[i]], _directory);
//             // _meshes.push_back(processMesh(mesh, scene));
//         }
//         // 接下来对它的子节点重复这一过程
//         for (unsigned int i = 0; i < node->mNumChildren; i++)
//         {
//             processNode(node->mChildren[i], scene);
//         }
//     }
//     Mesh processMesh(aiMesh* mesh, const aiScene* scene)
//     {
//         std::vector<Vertex> vertices;
//         std::vector<unsigned int> indices;
//         std::vector<Texture> textures;
//         for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//         {
//             Vertex vertex;
//             // 处理顶点位置、法线和纹理坐标
//             vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
//             vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
//             vertex.TexCoords = mesh->mTextureCoords[0]
//                                    ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
//                                    : glm::vec2{0, 0};
//             vertices.push_back(vertex);
//         }
//         for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//         {
//             aiFace face = mesh->mFaces[i];
//             for (unsigned int j = 0; j < face.mNumIndices; j++)
//             {
//                 indices.push_back(face.mIndices[j]);
//             }
//         }
//         if (mesh->mMaterialIndex >= 0)
//         {
//             TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], textures, _directory);
//         }
//         return Mesh{vertices, indices, textures};
//     }
// };
