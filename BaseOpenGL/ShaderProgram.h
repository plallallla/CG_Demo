#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>

using ShaderType = GLenum;
using ShaderID = GLuint;

/**
 * @brief 着色器程序 用于管理管线相关资源 绑定着色器
 * 
 */
class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(std::string_view vs, std::string_view fs);
    ShaderProgram(std::string_view vs, std::string_view gs,std::string_view fs);
    ~ShaderProgram();
    void load_shader_file(ShaderType type, std::string_view path);
    void load_shader_src(ShaderType type, std::string_view src);

    inline void load_vs_file(std::string_view path) { load_shader_file(GL_VERTEX_SHADER, path); }
    inline void load_vs_src(std::string_view src) { load_shader_src(GL_VERTEX_SHADER, src); }

    inline void load_fs_file(std::string_view path) { load_shader_file(GL_FRAGMENT_SHADER, path); }
    inline void load_fs_src(std::string_view src) { load_shader_src(GL_FRAGMENT_SHADER, src); }

    inline void load_gs_file(std::string_view path) { load_shader_file(GL_GEOMETRY_SHADER, path); }
    inline void load_gs_src(std::string_view src) { load_shader_src(GL_GEOMETRY_SHADER, src); }

    void link();
    void use() const;
    void active_samplers() const;
    void add_sampler(std::string_view sampler_name, const int& texture_id);
    int get_samplers_ct() const { return (int)_samplers.size(); }
    template<typename T>
    void set_uniform(std::string_view name, const T& value) const;
    inline GLuint get_id() const { return _id; };

private:
    GLuint _id;
    std::vector<unsigned int> _samplers;
};