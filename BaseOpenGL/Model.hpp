#pragma once
#include <string>

#include "ShaderProgram.h"
#include "Texture.hpp"
#include "Vertex.h"

class Mesh
{
  public:
    Vertexs _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;
    Mesh(Vertexs vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        : _vertices{vertices}, _indices{indices}, _textures{textures}
    {
        setupMesh();
    }
    void Draw(ShaderProgram& shader)
    {
        TEXTURE_MANAGER.active(shader, _textures);
        va.bind();
        glBindVertexArray(va._id);
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

  private:
    VertexArray va;
    void setupMesh()
    {
        va.bind();
        VertexBuffer vb;
        vb.set_data(_vertices.size() * sizeof(Vertex), _vertices.data());
        vb.add_layout(GL_FLOAT, 3, GL_FALSE);
        vb.add_layout(GL_FLOAT, 3, GL_FALSE);
        vb.add_layout(GL_FLOAT, 2, GL_FALSE);
        va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));
        va.setElementBuffer(_indices.data(), _indices.size() * sizeof(unsigned int));
        va.unbind();
    }
};

class Model
{
  public:
    Model(std::string path)
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
    std::vector<Mesh> _meshes;
    std::vector<Texture> _loaded_textures;
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
            _meshes.push_back(processMesh(mesh, scene));
        }
        // 接下来对它的子节点重复这一过程
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
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
        return Mesh{vertices, indices, textures};
    }
};
