#include "Vertex.h"

VertexDescription::VertexDescription(VertexType t, unsigned int c, bool n) : _type(t), _count(c), _normalized(n)
{    
}


unsigned int VertexDescription::type_size(unsigned int type)
{
    switch (type)
    {
        case GL_FLOAT: 
        {
            return 4;
        }
        case GL_UNSIGNED_INT: 
        {
            return 4;
        }
        case GL_UNSIGNED_BYTE: 
        {
            return 1;
        }
    }
    return 0;
}

void VertexBuffer::add_layout(VertexType type, unsigned int count, bool normalized)
{
    _layout._descriptions.emplace_back(type, count, normalized);
    _layout._stride += count * VertexDescription::type_size(type);
}

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &_id);
}

void VertexBuffer::set_data(size_t size, const void* Vertex, unsigned int usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, size, Vertex, usage);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_id);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &_id);
}

void VertexArray::free()
{
    glDeleteVertexArrays(1, &_id);
}

void VertexArray::setElementBuffer(const void* indices, unsigned int size, unsigned int usage)
{
    bind();
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    unbind();
}

void VertexArray::addVertexBuffer(const VertexBuffer_p& vb)
{
    bind();
    vb->bind();
    const auto& description = vb->getLayout().get_Vertex_descriptions();
    const auto& stride = vb->getLayout().get_stride();
    size_t offset = 0;
    for (int i = 0; i < description.size(); i++)
    {
        const auto& Vertex = description[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, Vertex._count, Vertex._type, Vertex._normalized, stride, (void*)offset);
        offset += (size_t)Vertex._count * VertexDescription::type_size(Vertex._type);
    }
    vb->unbind();
    unbind();
    _vertex_buffers.push_back(vb);
}

void VertexArray::bind() const
{
    glBindVertexArray(_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}