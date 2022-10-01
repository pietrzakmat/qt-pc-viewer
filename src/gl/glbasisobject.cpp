#include "glbasisobject.h"
#include <QColor>
#include <cmath> // M_PIf32


graphics::VertexData GLBasisObject::generate_vertex_data()
{
    graphics::VertexData vertices;

    vertices.positions.reserve(m_lines_vertex_count + m_triangles_count);

    // X segment
    vertices.positions.emplace_back(0.0F, 0.0F, 0.0F); vertices.colors.emplace_back(1.0F, 0.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(1.0F, 0.0F, 0.0F); vertices.colors.emplace_back(1.0F, 0.0F, 0.0F, 1.0F);
    // Y segment
    vertices.positions.emplace_back(0.0F, 0.0F, 0.0F); vertices.colors.emplace_back(0.0F, 1.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(0.0F, 1.0F, 0.0F); vertices.colors.emplace_back(0.0F, 1.0F, 0.0F, 1.0F);
    // Z segment
    vertices.positions.emplace_back(0.0F, 0.0F, 0.0F); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);
    vertices.positions.emplace_back(0.0F, 0.0F, 1.0F); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);

    constexpr float s = 0.05F;

    // Triangle 1
    vertices.positions.emplace_back(1.0F    ,  0.0F, 0.0F); vertices.colors.emplace_back(1.0F, 0.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(1.0F - s,  s   , 0.0F); vertices.colors.emplace_back(1.0F, 0.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(1.0F - s, -s   , 0.0F); vertices.colors.emplace_back(1.0F, 0.0F, 0.0F, 1.0F);

    // Triangle 2
    vertices.positions.emplace_back(0.0F , 1.0F    , 0.0F); vertices.colors.emplace_back(0.0F, 1.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(s    , 1.0F - s, 0.0F); vertices.colors.emplace_back(0.0F, 1.0F, 0.0F, 1.0F);
    vertices.positions.emplace_back(-s   , 1.0F - s, 0.0F); vertices.colors.emplace_back(0.0F, 1.0F, 0.0F, 1.0F);

    // Triangle 3
    vertices.positions.emplace_back(0.0F, 0.0F     , 1.0F); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);
    vertices.positions.emplace_back(s   , 0.0F,  1.0F - s); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);
    vertices.positions.emplace_back(-s   , 0.0F, 1.0F - s); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);

    // Triangle 4
    vertices.positions.emplace_back(0.0F, 0.0F, 1.0F    ); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);
    vertices.positions.emplace_back(0.0F,    s, 1.0F - s); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);
    vertices.positions.emplace_back(0.0F,   -s, 1.0F - s); vertices.colors.emplace_back(0.0F, 0.0F, 1.0F, 1.0F);

    return vertices;
}

void GLBasisObject::initialize_vertices(const graphics::VertexData& vertices)
{
    if (!m_shader) {
        std::cerr << __PRETTY_FUNCTION__  << " Error: No shader initialized\n";
        return;
    }

    if (m_vao)
        m_vao->destroy();

    m_vertices_count = vertices.positions.size();
    m_shader->bind();
    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create();
    m_vao->bind();
        std::unique_ptr<QOpenGLBuffer> vbo_position = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        vbo_position->create();
        vbo_position->bind();
        vbo_position->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo_position->allocate(vertices.positions.data(), static_cast<int>(vertices.positions.size() * sizeof (QVector3D))); // allocate with writing
        m_shader->setAttributeBuffer("vertex_position", GL_FLOAT, 0, 3);
        m_shader->enableAttributeArray("vertex_position");
        vbo_position->release();

        std::unique_ptr<QOpenGLBuffer> vbo_color = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        vbo_color->create();
        vbo_color->bind();
        vbo_color->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo_color->allocate(vertices.colors.data(), static_cast<int>(vertices.colors.size() * sizeof (QVector4D))); // allocate with writing
        m_shader->setAttributeBuffer("vertex_color", GL_FLOAT, 0, 4);
        m_shader->enableAttributeArray("vertex_color");
        vbo_color->release();

    m_vao->release();
    m_shader->release();

}

void GLBasisObject::initialize_gl()
{
    if (m_gl_initialized)
        return;

    initialize_vertices(generate_vertex_data());

    m_gl_initialized = true;

//    std::cerr << __PRETTY_FUNCTION__ << "\n";
//    std::cerr << __PRETTY_FUNCTION__ << ", m_vao.get(): " << m_vao.get() << "\n";
}

void GLBasisObject::draw()
{
    if (!m_gl_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }

    m_shader->bind();
        m_vao->bind();
            glDrawArrays(GL_LINES, 0, m_lines_vertex_count);
            glDrawArrays(GL_TRIANGLES, m_lines_vertex_count, m_triangles_count);
        m_vao->release();
    m_shader->release();
}
