#pragma once
#include <cstddef>
#include <string>

#include "ShaderProgram.h"
#include "Texture.hpp"
#include "Vertex.h"

#include "VertexArray.hpp"
#include "Buffer.hpp"

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
        // vva.bind();
        glBindVertexArray(vao);
        // glDrawElements(GL_TRIANGLES, _ct, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        vva.unbind();
    }
    vVertexArray vva;
    VertexArray va;
    size_t _ct;
    GLuint vao;
  private:
    void setupMesh()
    {
        // va.bind();
        // VertexBuffer vb;
        // vb.set_data(_vertices.size() * sizeof(Vertex), _vertices.data());
        // vb.add_layout(GL_FLOAT, 3, GL_FALSE);
        // vb.add_layout(GL_FLOAT, 3, GL_FALSE);
        // vb.add_layout(GL_FLOAT, 2, GL_FALSE);
        // va.addVertexBuffer(std::make_shared<VertexBuffer>(vb));
        // va.setElementBuffer(_indices.data(), _indices.size() * sizeof(unsigned int));
        // va.unbind();


        // GLuint vb_id = BUFFER.generate_buffer(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data());
        // GLuint eb_id = BUFFER.generate_buffer(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data());
        // vva.attach_buffer(PNT_LAYOUT, vb_id, eb_id);
        // _ct = static_cast<GLsizei>(_indices.size());

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        GLuint vb;
        glGenBuffers(1, &vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        GLuint eb;
        glGenBuffers(1, &eb);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
        glBindVertexArray(0);
    }
};

class Model
{
  public:
    std::vector<Mesh> _meshes;
    Model()
    {

    }
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
