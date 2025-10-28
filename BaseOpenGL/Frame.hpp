#pragma once
#include <glad/glad.h>
#include <stdexcept>

class FrameBuffer 
{
public:

    static void render_to_sreen()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    FrameBuffer(int width, int height) : _width(width), _height(height)
    {
        glGenFramebuffers(1, &_id);
    }
    
    ~FrameBuffer() 
    {
        if (_id) 
        {
            glDeleteFramebuffers(1, &_id);
        }
    }

    void attach_color_texture(GLuint texture)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _color_attachment_ct, GL_TEXTURE_2D, texture, 0);
        _color_attachment_ct++;
    }

    void attach_depth_texture(GLuint texture)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    }

    void use_render_object()
    {
        glGenRenderbuffers(1, &_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width * 2, _height * 2);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
    }

    void update_viewport() const
    {
        glViewport(0, 0, _width * 2, _height * 2);
    }

    inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _id); }
    inline void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    inline int get_width() const { return _width; }
    inline int get_height() const { return _height; }

    bool check_status() const
    {
        bind();
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        unbind();
        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    void checkFramebufferStatus() const
    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status)
        {
            case GL_FRAMEBUFFER_COMPLETE:
            {
                return;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            {
                throw std::runtime_error("Framebuffer error: INCOMPLETE_ATTACHMENT");
                return;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            {
                throw std::runtime_error("Framebuffer error: MISSING_ATTACHMENT");
                return;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            {
                throw std::runtime_error("Framebuffer error: INCOMPLETE_DRAW_BUFFER");
                return;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            {
                throw std::runtime_error("Framebuffer error: INCOMPLETE_READ_BUFFER");
                return;
            }
            case GL_FRAMEBUFFER_UNSUPPORTED:
            {
                throw std::runtime_error("Framebuffer error: UNSUPPORTED");
                return;
            }
            default:
            {
                throw std::runtime_error("Framebuffer error: UNKNOWN");
                return;
            }
        }
    }

private:
    int _width{ 0 };
    int _height{ 0 };
    GLuint _id;
    GLuint _rbo;
    int _color_attachment_ct{ 0 };
};


