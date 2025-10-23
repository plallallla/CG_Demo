#pragma once
#include <cstddef>
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
        va.bind();
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        va.unbind();
    }

    void draw_instance(ShaderProgram shader, unsigned int instance_id)
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
        va.bind();
        glBindVertexArray(va._id);
        // glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, instance_id);
        glBindVertexArray(0);
    }

    void add_vertexbuffer(const VertexBuffer& vb)
    {
        va.addVertexBuffer(vb);
    }

    void sets_attribute_divisor(unsigned int att_index, unsigned int divisor)
    {
        va.setAttributeDivisor(att_index, divisor);
    }

    void sets_attributes_divisor(const std::map<unsigned int, unsigned int>& indices_divisors)
    {
        va.setAttributesDivisor(indices_divisors);
    }

    VertexArray va;
  private:
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
    std::vector<Mesh> _meshes;
    std::vector<Texture> _loaded_textures;
    Model() = default;
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

    void draw_instance(ShaderProgram shader, unsigned int instance_id)
    {
        for (unsigned int i = 0; i < _meshes.size(); i++)
        {
            _meshes[i].draw_instance(shader, instance_id);
        }
    }

    void add_vertexbuffer(size_t index, const VertexBuffer& vb)
    {
        _meshes[index].add_vertexbuffer(vb);
    }

    void add_vertexbuffer(const VertexBuffer& vb)
    {
        for (auto& mesh : _meshes)
        {
            mesh.add_vertexbuffer(vb);
        }
    }

    void sets_attribute_divisor(unsigned int mesh_index, unsigned int att_index, unsigned int divisor)
    {
        _meshes[mesh_index].sets_attribute_divisor(att_index, divisor);
    }

    void sets_attribute_divisor(unsigned int att_index, unsigned int divisor)
    {
        for (auto& mesh : _meshes)
        {
            mesh.sets_attribute_divisor(att_index, divisor);
        }
    }

    void sets_attributes_divisor(unsigned int mesh_index, const std::map<unsigned int, unsigned int>& indices_divisors)
    {
        _meshes[mesh_index].sets_attributes_divisor(indices_divisors);
    }

    void sets_attributes_divisor(const std::map<unsigned int, unsigned int>& indices_divisors)
    {
        for (auto& mesh : _meshes)
        {
            mesh.sets_attributes_divisor(indices_divisors);
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
