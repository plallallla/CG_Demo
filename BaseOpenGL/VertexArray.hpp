#include <cstddef>
#include <glad/glad.h>
#include <vector>
#include <map>

struct VertexAttribute
{
    GLuint _type{ GL_FLOAT };
    GLuint _count{ 0 };
    bool _normalized{ false };
    bool _instanced{ false };
    GLuint _divisor{ 0 };
    VertexAttribute(GLuint type, GLuint count, bool normalized, bool instanced, GLuint divisor)
    : _type(type)
    , _count(count)
    , _normalized(normalized)
    , _instanced(instanced)
    , _divisor(divisor) 
    {}
    static GLuint get_type_length(GLuint type)
    {
        static std::map<GLuint, GLuint> type_length
        {
            { GL_FLOAT, 4 }, { GL_UNSIGNED_INT, 4}, { GL_UNSIGNED_BYTE, 1},
        };
        return type_length[type];
    }
};
using VertexAttributes = std::vector<VertexAttribute>;

struct BufferLayout
{
    VertexAttributes _attributes;
    GLuint _stride{ 0 };
    void add_attribute(GLuint type, GLuint count, bool normalized = true, bool instanced = false, GLuint divisor = 0)
    {

        _attributes.emplace_back(type, count, normalized, instanced, divisor);
        _stride += count * VertexAttribute::get_type_length(type);
    }

};

class VertexArray
{
    GLuint _id{ 0 };
    GLuint _attributes_ct{ 0 };

public:
    VertexArray() { glGenVertexArrays(1, &_id); }
    void bind() const { glBindVertexArray(_id); }
    void unbind() const { glBindVertexArray(0); }
    void attach_buffer(const BufferLayout& layout, GLsizeiptr size, const void *data, GLenum usage = GL_STATIC_DRAW)
    {
        glBindVertexArray(_id);
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        size_t offset = 0;
        for (size_t i = 0; i < layout._attributes.size(); i++)
        {
            const auto& attribute = layout._attributes[i];
            glVertexAttribPointer
            (
                _attributes_ct, 
                attribute._count, 
                attribute._type, 
                attribute._normalized ? GL_TRUE : GL_FALSE, 
                layout._stride, 
                (void*)offset
            );
            offset += (size_t)attribute._count * VertexAttribute::get_type_length(attribute._type);
            if (attribute._instanced) 
            {
                glVertexAttribDivisor(_attributes_ct, attribute._divisor);
            }
            glEnableVertexAttribArray(_attributes_ct++);
        }
        glBindVertexArray(0);
    }
};