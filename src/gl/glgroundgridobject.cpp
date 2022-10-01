#include "glgroundgridobject.h"

graphics::VertexData GLGroundGridObject::generate_vertex_data()
{
    graphics::VertexData vertices;
    vertices.positions.reserve(198);
    vertices.colors.reserve(198);

    constexpr float size = 0.01F;
    for (int i=-99; i<=99; i++)
    {
        float v = (100.0F - static_cast<float>(qAbs(i))) / 100.0F;
        if (i%10 != 0)
        {
            if (i%5 == 0)
                v /= 2.0F;
            else
                v /= 4.0F;
        }
        v *= 0.8F;

        float shift = static_cast<float>(i) * size;

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F,v);
        vertices.positions.emplace_back(shift, 0.0F, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F,0.0F);
        vertices.positions.emplace_back(shift, 99 * size, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F,v);
        vertices.positions.emplace_back(shift, 0.0F, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F, 0.0F);
        vertices.positions.emplace_back(shift,-99 * size, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F,v);
        vertices.positions.emplace_back( 0.0F      , shift, 0.0F);

        vertices.colors.emplace_back(1.0F, .0F, 1.0F, 0.0F);
        vertices.positions.emplace_back( 99 * size, shift, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F,v);
        vertices.positions.emplace_back(0.0F      , shift, 0.0F);

        vertices.colors.emplace_back(1.0F, 1.0F, 1.0F, 0.0F);
        vertices.positions.emplace_back(-99 * size, shift, 0.0F);
    }

    return vertices;
}

void GLGroundGridObject::initialize_vertices(const graphics::VertexData& vertex_data)
{
    if (!m_shader) {
        std::cerr << __PRETTY_FUNCTION__  << " Error: No shader initialized\n";
        return;
    }

    if (m_vao)
        m_vao->destroy();

    m_vertices_count = vertex_data.positions.size();
    m_shader->bind();
    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create();
    m_vao->bind();
        std::unique_ptr<QOpenGLBuffer> vbo_position = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        vbo_position->create();
        vbo_position->bind();
        vbo_position->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo_position->allocate(vertex_data.positions.data(), static_cast<int>(vertex_data.positions.size() * sizeof (QVector3D))); // allocate with writing
        m_shader->setAttributeBuffer("vertex_position", GL_FLOAT, 0, 3);
        m_shader->enableAttributeArray("vertex_position");
        vbo_position->release();

        std::unique_ptr<QOpenGLBuffer> vbo_color = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        vbo_color->create();
        vbo_color->bind();
        vbo_color->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo_color->allocate(vertex_data.colors.data(), static_cast<int>(vertex_data.colors.size() * sizeof (QVector4D))); // allocate with writing
        m_shader->setAttributeBuffer("vertex_color", GL_FLOAT, 0, 4);
        m_shader->enableAttributeArray("vertex_color");
        vbo_color->release();

    m_vao->release();
    m_shader->release();
}

void GLGroundGridObject::initialize_gl()
{
    if (m_gl_initialized)
        return;

    initialize_vertices(generate_vertex_data());

    m_gl_initialized = true;
}

void GLGroundGridObject::draw(const QColor& color)
{
    if (!m_gl_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }

    m_shader->bind();
        m_vao->bind();
        m_shader->setAttributeValue("main_color", color);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices_count));
        m_vao->release();
    m_shader->release();
}
