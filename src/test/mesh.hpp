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

#include "Vertex.h"

class vMesh
{
  public:
    Vertexs _vertices;
    std::vector<GLuint> _indices;
    std::vector<Texture> _textures;
    vMesh(aiMesh* mesh, const aiScene* scene, std::string_view directory)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            // 处理顶点位置、法线和纹理坐标
            vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            vertex.TexCoords = mesh->mTextureCoords[0]
                                   ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                                   : glm::vec2{0, 0};
            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        GLuint vb = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, vertices.size() * sizeof(float) * 8, vertices.data());
        GLuint eb = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data());
        vva.attach_buffer(PNT_LAYOUT, vb, eb);
        if (mesh->mMaterialIndex >= 0)
        {
            TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], _textures, directory);
        }
        _ct = indices.size();
    }
    void Draw(ShaderProgram& shader) const
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
        // glBindVertexArray(vao);
        vva.bind();
        glDrawElements(GL_TRIANGLES, fmax(_ct, _indices.size()), GL_UNSIGNED_INT, 0);
        vva.unbind();
    }
    vVertexArray vva;
    size_t _ct;
    GLuint vao;
};

class vModel
{
  public:
    std::vector<vMesh> _meshes;
    vModel()
    {

    }
    vModel(std::string path)
    {
        loadModel(path);
    }
    void Draw(ShaderProgram shader)
    {
        for (unsigned int i = 0; i < _meshes.size(); i++)
        {
            _meshes[i].Draw(shader);
        }

    }



  private:
    std::string _directory;
    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            // cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
            return;
        }
        _directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    void processNode(aiNode* node, const aiScene* scene)
    {
        // 处理节点所有的网格（如果有的话）
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            // _meshes.push_back(processMesh(mesh, scene));
            _meshes.emplace_back(mesh, scene, _directory);
        }
        // 接下来对它的子节点重复这一过程
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }
    vMesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            // 处理顶点位置、法线和纹理坐标
            vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            vertex.TexCoords = mesh->mTextureCoords[0]
                                   ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                                   : glm::vec2{0, 0};
            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        if (mesh->mMaterialIndex >= 0)
        {
            TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], textures, _directory);
        }
        return vMesh{vertices, indices, textures};
    }
};
