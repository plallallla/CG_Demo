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
    VertexArray _va;
    Textures _textures;
    GLsizei _ct{ 0 };

    void set_up_va(const aiScene* scene, const aiMesh* mesh)
    {
        std::vector<float> vertices;
        std::vector<GLuint> indices;
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
        if (mesh->mMaterialIndex >= 0)
        {
            TEXTURE_MANAGER.load_from_material(scene->mMaterials[mesh->mMaterialIndex], _textures, directory);
        }
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
    std::vector<Mesh> _meshes;
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
        for (unsigned int i = 0; i < _meshes.size(); i++)
        {
            _meshes[i].render_elements(sp);
        }
    }
};