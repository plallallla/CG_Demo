#pragma once
#include <glad/glad.h>

struct FilterMode
{
    GLuint _min = GL_LINEAR;
    GLuint _max = GL_LINEAR;
};

struct WrapMode
{
    GLuint _s = GL_REPEAT;
    GLuint _t = GL_REPEAT;
    GLuint _r = GL_REPEAT;
};

struct TextureFormat
{
    GLuint _internal = GL_RGB8;
    GLuint _image    = GL_RGB;
    GLuint _pixel    = GL_UNSIGNED_BYTE;
};

struct TextureAttributes
{
    GLuint _target{ GL_TEXTURE_2D };
    FilterMode _filter;
    WrapMode _wrap;
    TextureFormat _format;
    bool _mipmap{ true };
};

inline auto TEXTURE_2D_GRAY = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                              
        {GL_LINEAR, GL_LINEAR},                     
        {GL_REPEAT, GL_REPEAT},                     
        {GL_R8, GL_RED, GL_UNSIGNED_BYTE},        
        true                                        
    };
}();

inline auto TEXTURE_2D_RGB = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                              
        {GL_LINEAR, GL_LINEAR},                     
        {GL_REPEAT, GL_REPEAT},                     
        {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE},        
        true                                        
    };
}();

inline auto TEXTURE_2D_RGBA = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                              
        {GL_LINEAR, GL_LINEAR},                     
        {GL_REPEAT, GL_REPEAT},                     
        {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE},        
        true                                        
    };
}();

inline auto TEXTURE_2D_RGBA16F = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                              
        {GL_LINEAR, GL_LINEAR},                     
        {GL_REPEAT, GL_REPEAT},                     
        {GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE},    
        true                                        
    };
}();

inline auto TEXTURE_2D_SRGB = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                             
        {GL_LINEAR, GL_LINEAR},                    
        {GL_REPEAT, GL_REPEAT},
        {GL_SRGB8, GL_SRGB, GL_UNSIGNED_BYTE},
        true     
    };
}();

inline auto TEXTURE_2D_GAMMA = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,                              // target
        {GL_LINEAR, GL_LINEAR},                     // filter
        {GL_REPEAT, GL_REPEAT},                     // wrap
        {GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE},    // format
        true                                        // mipmap
    };
}();

inline auto TEXTURE_2D_GAMMA_ALPHA = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,
        {GL_LINEAR, GL_LINEAR},
        {GL_REPEAT, GL_REPEAT},
        {GL_SRGB_ALPHA, GL_RGBA, GL_UNSIGNED_BYTE},
        true
    };
}();

inline auto TEXTURE_2D_DEPTH = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,
        {GL_NEAREST, GL_NEAREST},
        {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
        {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT},
        true
    };
}();

inline auto TEXTURE_2D_HDR = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_2D,
        {GL_NEAREST, GL_NEAREST},
        {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
        {GL_RGBA16F, GL_RGBA, GL_FLOAT},
        true
    };
}();

inline auto TEXTURE_CUBE_RGB = []() -> TextureAttributes {
    return
    {
        GL_TEXTURE_CUBE_MAP,
        {GL_NEAREST, GL_NEAREST},
        {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
        {GL_RGBA16F, GL_RGBA, GL_FLOAT},
        false
    };
}();

