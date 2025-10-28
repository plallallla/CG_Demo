#pragma once
#include <glad/glad.h>

struct TextureAttributes
{
    GLuint _target = GL_TEXTURE_2D;
    GLuint _wrap_s = GL_REPEAT;
    GLuint _wrap_t = GL_REPEAT;
    GLuint _min_filtering = GL_LINEAR;
    GLuint _max_filtering = GL_LINEAR;
    GLuint _internal_format = GL_RGB;
    GLuint _image_format = GL_RGB;
    GLuint _pixel_type = GL_UNSIGNED_BYTE;
    bool _need_mipmap = true;
};

inline auto TEXTURE_2D_RGB = [] () -> TextureAttributes
{
    return TextureAttributes
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

inline auto TEXTURE_2D_SRGB = [] () -> TextureAttributes
{
    return TextureAttributes
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

inline auto TEXTURE_2D_GAMMA = [] () -> TextureAttributes
{
    return TextureAttributes
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

inline auto TEXTURE_2D_ALPHA = [] () -> TextureAttributes
{
    return TextureAttributes
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

inline auto TEXTURE_2D_GRAY = []() -> TextureAttributes {
    return TextureAttributes
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