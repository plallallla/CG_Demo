#pragma once
#include <assimp/material.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "utility.hpp"
#include "TextureAttributes.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture
{
    GLuint _id{0};
    std::string _type{""};
    Texture(GLuint id = 0, std::string_view type = "") : _id{ id }, _type{ type } {}
    operator GLuint() const
    {
        return _id;
    }
};
using Textures = std::vector<Texture>;

#define TEXTURE_MANAGER TextureManager::getInstance()
class TextureManager
{
    SINGLETON(TextureManager);
    std::unordered_map<std::string, unsigned int> _loaded_textures;
public:
    void load_from_material(aiMaterial* material, Textures& textures, std::string_view directory)
    {
        auto diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        auto normalMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_normal", directory);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        auto heightMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_height", directory);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    Textures load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string_view directory)
    {
        Textures textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString file_name;
            mat->GetTexture(type, i, &file_name);
            std::string path = std::string{directory.data()} + "/" + file_name.C_Str();
            textures.emplace_back(auto_load_texture(path), typeName);
        }
        return textures;
    }

    GLuint auto_load_texture(std::string_view path)
    {
        if (_loaded_textures.find(path.data()) != _loaded_textures.end())
        {
            return _loaded_textures[path.data()];
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        TextureAttributes attributes;
        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
        if (!data)
        {
            LOG.info(std::string{"Texture load error : "} + path.data());
            stbi_image_free(data);
        }
        switch (nrComponents) 
        {
            case 1:
            {
                attributes = TEXTURE_2D_GRAY;
                break;
            }
            case 3:
            {
                attributes = TEXTURE_2D_RGB;
                break;
            }
            case 4:
            {
                attributes = TEXTURE_2D_ALPHA;
                break;
            }
            default:
            {
                throw std::runtime_error("we cannot support this components");
            }
        }
        glBindTexture(attributes._target, textureID);
        glTexImage2D
        (
            GL_TEXTURE_2D, 
            0, 
            attributes._internal_format, 
            width, 
            height, 
            0, 
            attributes._image_format, 
            GL_UNSIGNED_BYTE, 
            data
        );
        if (attributes._need_mipmap) glGenerateMipmap(attributes._target);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_S, attributes._wrap_s);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_T, attributes._wrap_t);
        glTexParameteri(attributes._target, GL_TEXTURE_MIN_FILTER, attributes._min_filtering);
        glTexParameteri(attributes._target, GL_TEXTURE_MAG_FILTER, attributes._max_filtering);
        glBindTexture(attributes._target, textureID);
        _loaded_textures[path.data()] = textureID;
        return textureID;
    }

    GLuint load_texture(std::string_view path, const TextureAttributes& attributes = TEXTURE_2D_RGB)
    {
        if (_loaded_textures.find(path.data()) != _loaded_textures.end())
        {
            return _loaded_textures[path.data()];
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(attributes._target, textureID);
        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
        if (!data)
        {
            LOG.info(std::string{"Texture load error : "} + path.data());
            stbi_image_free(data);
            return 0;
        }
        glTexImage2D
        (
            GL_TEXTURE_2D, 
            0, 
            attributes._internal_format, 
            width, 
            height, 
            0, 
            attributes._image_format, 
            GL_UNSIGNED_BYTE, 
            data
        );
        if (attributes._need_mipmap) glGenerateMipmap(attributes._target);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_S, attributes._wrap_s);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_T, attributes._wrap_t);
        glTexParameteri(attributes._target, GL_TEXTURE_MIN_FILTER, attributes._min_filtering);
        glTexParameteri(attributes._target, GL_TEXTURE_MAG_FILTER, attributes._max_filtering);
        glBindTexture(attributes._target, textureID);
        _loaded_textures[path.data()] = textureID;
        stbi_image_free(data);
        return textureID;
    }

    GLuint generate_texture_buffer(int width, int height, const TextureAttributes& attributes)
    {
        GLuint id;
        glGenTextures(1, &id);
        glBindTexture(attributes._target, id);
        glTexImage2D(attributes._target, 0, attributes._internal_format, width, height, 0, attributes._image_format, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_S, attributes._wrap_s);
        glTexParameteri(attributes._target, GL_TEXTURE_WRAP_T, attributes._wrap_t);
        glTexParameteri(attributes._target, GL_TEXTURE_MIN_FILTER, attributes._min_filtering);
        glTexParameteri(attributes._target, GL_TEXTURE_MAG_FILTER, attributes._max_filtering);
        return id;
    }
};


