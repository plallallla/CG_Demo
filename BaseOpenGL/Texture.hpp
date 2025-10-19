#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "utility.hpp"
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture
{
    unsigned int id{0};
    std::string type{""};
    Texture(unsigned int Id = 0, std::string Type = "") : id(Id), type(Type) {}
};
using Textures = std::vector<Texture>;

struct TextureAttributes
{
    unsigned int _target = GL_TEXTURE_2D;
    unsigned int _wrap_s = GL_REPEAT;
    unsigned int _wrap_t = GL_REPEAT;
    unsigned int _min_filtering = GL_LINEAR;
    unsigned int _max_filtering = GL_LINEAR;
    unsigned int _internal_format = GL_RGB;
    unsigned int _pixel_type = GL_UNSIGNED_BYTE;
    bool _need_mipmap = true;
};

#define TEXTURE_MANAGER TextureManager::getInstance()
class TextureManager
{
    SINGLETON(TextureManager);
    std::unordered_map<std::string, unsigned int> _loaded_textures;
public:
    void load_from_material(aiMaterial* material, Textures& textures, std::string_view directory)
    {
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    Textures loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string_view directory)
    {
        Textures textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString file_name;
            mat->GetTexture(type, i, &file_name);
            TextureAttributes att;
            std::string path = std::string{directory.data()} + "/" + file_name.C_Str();
            textures.emplace_back(load_texture(path, att), typeName);
        }
        return textures;
    }
    unsigned int load_texture(std::string_view path, const TextureAttributes& attributes = TextureAttributes())
    {
        if (_loaded_textures.find(path.data()) != _loaded_textures.end())
        {
            return _loaded_textures[path.data()];
        }
        unsigned int textureID;
        glGenTextures(1, &textureID);
        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
            {
                format = GL_RED;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                format = GL_RGBA;
            }
            glBindTexture(attributes._target, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            if (attributes._need_mipmap) 
            {
                glGenerateMipmap(attributes._target);
            }
            glTexParameteri(attributes._target, GL_TEXTURE_WRAP_S, attributes._wrap_s);
            glTexParameteri(attributes._target, GL_TEXTURE_WRAP_T, attributes._wrap_t);
            glTexParameteri(attributes._target, GL_TEXTURE_MIN_FILTER, attributes._min_filtering);
            glTexParameteri(attributes._target, GL_TEXTURE_MAG_FILTER, attributes._max_filtering);
            glBindTexture(GL_TEXTURE_2D, textureID);

            stbi_image_free(data);
        }
        else
        {
            // std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
        _loaded_textures[path.data()] = textureID;
        return textureID;
    }
};

