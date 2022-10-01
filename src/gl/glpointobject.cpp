#include "glpointobject.h"

void GLPointObject::initialize_gl()
{
    if (m_gl_initialized)
        return;

    m_gl_initialized = true;
}

void GLPointObject::draw()
{
    if (!m_gl_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }
    if (!m_shader) {
        std::cerr << __PRETTY_FUNCTION__  << " Error: No shader initialized\n";
        return;
    }

    m_shader->bind();
        m_vao->bind();
        glPointSize(m_point_size);
        glEnable(GL_POINT_SMOOTH); // draws rounded points
        glDrawArrays(GL_POINTS, 0, 1);
        glDisable(GL_POINT_SMOOTH);
        m_vao->release();
    m_shader->release();
}

void GLPointObject::set_point(const graphics::VertexData &vertex_data)
{
    if (!m_gl_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }

    if (nullptr == m_shader){
        std::cerr << "GLPointObject::set_points shader is not created, doing nothing\n";
        return;
    }

    if (m_vao)
        m_vao->destroy();

    m_shader->bind();
    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create();
    m_vao->bind();

    // Positions
    std::unique_ptr<QOpenGLBuffer> vbo_position = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vbo_position->create();
    vbo_position->bind();
    vbo_position->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_position->allocate(vertex_data.positions.data(), static_cast<int>(vertex_data.positions.size() * sizeof (QVector3D))); // allocate with writing
    m_shader->setAttributeBuffer("vertex_position", GL_FLOAT, 0, 3);
    m_shader->enableAttributeArray("vertex_position");
    vbo_position->release();

    // Colors
    std::unique_ptr<QOpenGLBuffer> vbo_color = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vbo_color->create();
    vbo_color->bind();
    vbo_color->setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (vertex_data.colors.size() == 0){
        std::vector<QVector4D> white = {QVector4D(1,1,1,1)};
        vbo_color->allocate(white.data(), static_cast<int>(white.size() * sizeof (QVector4D))); // allocate with writing
    } else {
        vbo_color->allocate(vertex_data.colors.data(), static_cast<int>(vertex_data.colors.size() * sizeof (QVector4D))); // allocate with writing
    }
    m_shader->setAttributeBuffer("vertex_color", GL_FLOAT, 0, 4);
    m_shader->enableAttributeArray("vertex_color");
    vbo_color->release();

    m_vao->release();
    m_shader->release();

    m_gl_initialized = true;
}
