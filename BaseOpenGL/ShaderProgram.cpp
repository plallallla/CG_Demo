#include "ShaderProgram.h"
#include "Shader.hpp"
#include "utility.hpp"

ShaderProgram::ShaderProgram()
{
    _id = glCreateProgram();
}

ShaderProgram::ShaderProgram(std::string_view vs, std::string_view fs)
{
    _id = glCreateProgram();
    load_shader_file(GL_VERTEX_SHADER, vs);
    load_shader_file(GL_FRAGMENT_SHADER, fs);
    link();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_id);
    _id = 0;
}

void ShaderProgram::load_shader_file(ShaderType type, std::string_view path)
{
    std::string src;
    utility::read_file(path, src);
    load_shader_src(type, src);
}

void ShaderProgram::load_shader_src(ShaderType type, std::string_view src)
{
    Shader s{ type };
    s.init_from_src(src);
    glAttachShader(_id, s.id());
}

void ShaderProgram::link()
{
    glLinkProgram(_id);
    utility::checkCompileErrors(_id, "PROGRAME");
}

void ShaderProgram::use() const
{ 
    glUseProgram(_id); 
}

template<>
void ShaderProgram::set_uniform<float>(std::string_view name, const float& value) const
{
    glUniform1f(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set_uniform<int>(std::string_view name, const int& value) const
{
    glUniform1i(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set_uniform<bool>(std::string_view name, const bool& value) const
{
    glUniform1i(glGetUniformLocation(_id, name.data()), value ? 1 : 0);
}

template<>
void ShaderProgram::set_uniform<unsigned int>(std::string_view name, const unsigned int& value) const
{
    glUniform1ui(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set_uniform<glm::vec2>(std::string_view name, const glm::vec2& value) const
{
    glUniform2f(glGetUniformLocation(_id, name.data()), value.x, value.y);
}

template<>
void ShaderProgram::set_uniform<glm::vec3>(std::string_view name, const glm::vec3& value) const
{
    glUniform3f(glGetUniformLocation(_id, name.data()), value.x, value.y, value.z);
}

template<>
void ShaderProgram::set_uniform<glm::vec4>(std::string_view name, const glm::vec4& value) const
{
    glUniform4f(glGetUniformLocation(_id, name.data()), value.x, value.y, value.z, value.w);
}

template<>
void ShaderProgram::set_uniform<glm::mat2>(std::string_view name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(_id, name.data()), 1, GL_FALSE, &mat[0][0]);
}

template<>
void ShaderProgram::set_uniform<glm::mat3>(std::string_view name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(_id, name.data()), 1, GL_FALSE, &mat[0][0]);
}

template<>
void ShaderProgram::set_uniform<glm::mat4>(std::string_view name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(_id, name.data()), 1, GL_FALSE, &mat[0][0]);
}
