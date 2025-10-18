#pragma once
#include <glad/glad.h>

#include <vector>
#include <stdexcept>

class FrameBuffer 
{
public:
    FrameBuffer(int width, int height) : _width(width), _height(height)
    {
        // 生成 FBO
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        // 创建颜色和深度附件
        createColorAttachments();
        createDepthAttachment();
        // 检查完整性
        checkFramebufferStatus();
        // 解绑
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    ~FrameBuffer() 
    {
        if (_fbo) 
        {
            glDeleteFramebuffers(1, &_fbo);
        }
        if (_colorTextures)
        {
            glDeleteTextures(1, &_colorTextures);
        }
        if (_depthTexture) 
        {
            glDeleteTextures(1, &_depthTexture);
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
    void bindForRead() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
    }
    // 绑定为绘制帧缓冲（用于 blit）
    void bindForDraw() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    }

    // 设置视口为 FBO 尺寸
    void setViewport() const 
    {
        glViewport(0, 0, _width, _height);
    }

    unsigned int getColorTexture() const { return _colorTextures; };
    
    unsigned int getDepthTexture() const { return _depthTexture; }

    // 手动检查 FBO 是否完整
    bool checkStatus() const
    {
        bind();
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        unbind();
        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    // 获取尺寸
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

private:
    unsigned int _fbo = 0;
    unsigned int _colorTextures;
    unsigned int _depthTexture = 0;

    int _width = 0;
    int _height = 0;

    void createColorAttachments()
    {
        glGenTextures(1, &_colorTextures);
        glBindTexture(GL_TEXTURE_2D, _colorTextures);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // 附加到 FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTextures, 0);
    }

    void createDepthAttachment()
    {
        glGenTextures(1, &_depthTexture);
        glBindTexture(GL_TEXTURE_2D, _depthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
    }

    void checkFramebufferStatus() const
    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status) 
        {
            case GL_FRAMEBUFFER_COMPLETE:
                return;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                throw std::runtime_error("Framebuffer error: INCOMPLETE_ATTACHMENT");
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                throw std::runtime_error("Framebuffer error: MISSING_ATTACHMENT");
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                throw std::runtime_error("Framebuffer error: INCOMPLETE_DRAW_BUFFER");
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                throw std::runtime_error("Framebuffer error: INCOMPLETE_READ_BUFFER");
            case GL_FRAMEBUFFER_UNSUPPORTED:
                throw std::runtime_error("Framebuffer error: UNSUPPORTED");
            default:
                throw std::runtime_error("Framebuffer error: UNKNOWN");
        }
    }
};


