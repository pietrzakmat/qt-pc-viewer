#include "glcameraobject.h"

graphics::VertexData GLCameraObject::generate_vertex_data()
{
    graphics::VertexData vertices;
    float w_d2 = 0.25f;
    float h_d2 = 0.25f;

    vertices.positions = {
        QVector3D(-w_d2, h_d2, 1.0F),
        QVector3D(w_d2, h_d2 , 1.0F),
        QVector3D(w_d2 ,-h_d2, 1.0F),
        QVector3D(-w_d2,-h_d2, 1.0F),

        QVector3D(-w_d2, h_d2, 1.0F),
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(-w_d2, -h_d2, 1.0F),

        QVector3D(w_d2, h_d2, 1.0F),
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(w_d2, -h_d2, 1.0F),
    };

    vertices.colors = {
        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),

        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),

        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),
        QVector4D(1.0, 1.0, 1.0, 1.0),
    };

    return vertices;
}

void GLCameraObject::initialize_gl()
{
    if (m_gl_initialized)
        return;

    initialize_vertices(generate_vertex_data());

    m_gl_initialized = true;
}

void GLCameraObject::initialize_vertices(const graphics::VertexData& vertex_data)
{
    if (!m_shader) {
        std::cerr << __PRETTY_FUNCTION__  << " Error: No shader initialized\n";
        return;
    }

    if (m_vao)
        m_vao->destroy();

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

void GLCameraObject::draw(const QColor& color)
{
    if (!m_gl_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }
    m_shader->bind();
        m_vao->bind();
        m_shader->setAttributeValue("main_color", color);
        glDrawArrays(GL_LINE_LOOP, m_frame1, 4);
        glDrawArrays(GL_LINE_STRIP, m_frame2, 3);
        glDrawArrays(GL_LINE_STRIP, m_frame3, 3);
        m_vao->release();
    m_shader->release();
}
