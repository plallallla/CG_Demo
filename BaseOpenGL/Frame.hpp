#pragma once
#include <glad/glad.h>

#include <stdexcept>

class FrameBuffer 
{
public:
    FrameBuffer(int width, int height) : _width(width), _height(height)
    {
        glGenFramebuffers(1, &_fbo);
    }
    
    ~FrameBuffer() 
    {
        if (_fbo) 
        {
            glDeleteFramebuffers(1, &_fbo);
        }
        if (_color_texture)
        {
            glDeleteTextures(1, &_color_texture);
        }
        if (_depth_texture) 
        {
            glDeleteTextures(1, &_depth_texture);
        }
    }
    void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    }
    void unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    // 绑定为读取帧缓冲（用于 blit 或 read pixels）
    void bind_for_read() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
    }
    // 绑定为绘制帧缓冲（用于 blit）
    void bind_for_draw() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    }

    // 设置视口为 FBO 尺寸
    void update_viewport() const 
    {
        glViewport(0, 0, _width*2, _height*2);
    }

    unsigned int get_color_texture() const { return _color_texture; };
    unsigned int get_depth_texture() const { return _depth_texture; };

    // 手动检查 FBO 是否完整
    bool check_status() const
    {
        bind();
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        unbind();
        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    // 获取尺寸
    int get_width() const { return _width; }
    int get_height() const { return _height; }
    // 创建深度纹理附件
    void create_depth_attachment()
    {
        //创建深度纹理
        glGenTextures(1, &_depth_texture);
        glBindTexture(GL_TEXTURE_2D, _depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _width*2, _height*2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    // 创建颜色纹理附件
    void create_color_texture_attachment()
    {
        //创建颜色纹理
        glGenTextures(1, &_color_texture);
        glBindTexture(GL_TEXTURE_2D, _color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color_texture, 0);
        unbind();
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
    unsigned int _fbo = 0;
    unsigned int _color_texture = 0;
    unsigned int _depth_texture = 0;

    int _width = 0;
    int _height = 0;
};


