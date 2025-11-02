#pragma once
#include <glad/glad.h>

struct TextureAttributes2D
{
    GLuint _target = GL_TEXTURE_2D;
    GLuint _wrap_s = GL_REPEAT;
    GLuint _wrap_t = GL_REPEAT;
    GLuint _filtering_min = GL_LINEAR;
    GLuint _filtering_max = GL_LINEAR;
    GLuint _internal_format = GL_RGB;
    GLuint _image_format = GL_RGB;
    GLuint _pixel_type = GL_UNSIGNED_BYTE;
    bool _need_mipmap = true;
};

inline auto TEXTURE_2D_RGB = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_RGB8,
        GL_RGB, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_RGBA16F = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_RGBA16F,
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_SRGB = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_SRGB8,
        GL_SRGB, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_GAMMA = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_SRGB8,
        GL_RGB, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_GAMMA_ALPHA = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_SRGB_ALPHA,
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_RGBA = [] () -> TextureAttributes2D
{
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_RGBA8,
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        true
    };
}();

inline auto TEXTURE_2D_GRAY = []() -> TextureAttributes2D {
    return
    {
        GL_TEXTURE_2D,
        GL_REPEAT, GL_REPEAT, // warp
        GL_LINEAR, GL_LINEAR, // filter
        GL_R8, 
        GL_RED,
        GL_UNSIGNED_BYTE,
        false
    };
}();

inline auto TEXTURE_2D_DEPTH = []() -> TextureAttributes2D {
    return
    {
        GL_TEXTURE_2D,
        GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, // warp
        GL_NEAREST, GL_NEAREST, // filter
        GL_DEPTH_COMPONENT24, 
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        false
    };
}();

inline auto TEXTURE_2D_HDR = []() -> TextureAttributes2D {
    return
    {
        GL_TEXTURE_2D,
        GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, // warp
        GL_NEAREST, GL_NEAREST, // filter
        GL_RGBA16F, 
        GL_RGBA,
        GL_FLOAT,
        false
    };
}();

struct TextureAttributesCubeMap
{
    GLuint _target = GL_TEXTURE_CUBE_MAP;
    GLuint _wrap_s = GL_REPEAT;
    GLuint _wrap_t = GL_REPEAT;
    GLuint _wrap_r = GL_REPEAT;
    GLuint _filtering_min = GL_LINEAR;
    GLuint _filtering_max = GL_LINEAR;
    GLuint _internal_format = GL_RGB;
    GLuint _image_format = GL_RGB;
    GLuint _pixel_type = GL_UNSIGNED_BYTE;
    bool _need_mipmap = true;
};

inline auto TEXTURE_CUBE_RGB = []() -> TextureAttributesCubeMap {
    return
    {
        GL_TEXTURE_2D,
        GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, // warp
        GL_NEAREST, GL_NEAREST, // filter
        GL_RGBA16F, 
        GL_RGBA,
        GL_FLOAT,
        false
    };
}();