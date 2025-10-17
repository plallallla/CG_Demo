#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>

using VertexType = unsigned int;
using BufferType = unsigned int;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
using Vertexs = std::vector<Vertex>;

struct VertexDescription
{
    VertexType _type;
    unsigned int _count;
    bool _normalized;
    VertexDescription(VertexType, unsigned int, bool);
    static unsigned int type_size(VertexType type);
};
using VertexDescriptions = std::vector<VertexDescription>;

struct VertexLayout
{
    VertexLayout() = default;
    inline const VertexDescriptions& get_Vertex_descriptions() const { return _descriptions; }
    inline unsigned int get_stride() const { return _stride; }
    VertexDescriptions _descriptions;
    unsigned int _stride = 0;
};

class VertexBuffer
{
private:
    unsigned int _id;
    VertexLayout _layout;

public:
    VertexBuffer();
    ~VertexBuffer();

    void set_data(size_t size, const void* Vertex, unsigned int usage = GL_STATIC_DRAW);
    void add_layout(VertexType, unsigned int, bool);

    void bind() const;
    void unbind() const;

    const VertexLayout& getLayout() const { return _layout; }
};
using VertexBuffer_p = std::shared_ptr<VertexBuffer>;
using VertexBuffer_ps = std::vector<VertexBuffer_p>;

class VertexArray
{
private:
    unsigned int _ebo{0};
    VertexBuffer_ps _vertex_buffers;

public:
    unsigned int _id;
    VertexArray();

    void setElementBuffer(const void* indices, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
    void addVertexBuffer(const VertexBuffer_p& vb);
    void bind() const;
    void unbind() const;
    void free();
};