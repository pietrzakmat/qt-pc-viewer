#include "glpointcloudobject.h"

void GLPointCloudObject::initialize_gl()
{
    if (m_initialized)
        return;


    m_initialized = true;
}


QMatrix4x4 GLPointCloudObject::get_model_mat()
{
    QMatrix4x4 m;
    m.translate(m_offset);
    m.scale(m_scale);
    m.rotate(m_rotate.x(), 1,0,0);
    m.rotate(m_rotate.y(), 0,1,0);
    m.rotate(m_rotate.z(), 0,0,1);

    return m;
}

void GLPointCloudObject::draw(const float point_size)
{
    if (!m_initialized || !m_shader) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }

//    std::cerr << __PRETTY_FUNCTION__ << " m_vertices_count= " << m_vertices_count << "\n";
    m_shader->bind();
        m_vao->bind();
//        glPointSize(m_point_size);
        m_shader->setAttributeValue("main_color", QColor(255,255,255));
        glPointSize(point_size);
        glEnable(GL_POINT_SMOOTH); // draws rounded points
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_vertices_count));
        glDisable(GL_POINT_SMOOTH);
        m_vao->release();
    m_shader->release();
}

void GLPointCloudObject::set_points(const graphics::VertexData &vertex_data)
{
    if (!m_initialized) {
        std::cerr << __PRETTY_FUNCTION__ << " not initialized\n";
        return;
    }

    if (nullptr == m_shader){
        std::cerr << "GLPointCloudObject::set_points shader is not created, doing nothing\n";
        return;
    }
    m_vertices_count = vertex_data.positions.size();

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
        if (m_x_inversion || m_y_inversion || m_z_inversion) { // inverse x,y,z
            std::vector<QVector3D> positions; positions.reserve(positions.size());
            const float factor_x = m_x_inversion ? -1.F : 1.F;
            const float factor_y = m_y_inversion ? -1.F : 1.F;
            const float factor_z = m_z_inversion ? -1.F : 1.F;
            for (const auto &p : vertex_data.positions) {
                positions.emplace_back(factor_x*p.x(), factor_y*p.y(), factor_z*p.z());
            }
            vbo_position->allocate(positions.data(), static_cast<int>(positions.size() * sizeof (QVector3D))); // allocate with writing
        } else {
            vbo_position->allocate(vertex_data.positions.data(), static_cast<int>(vertex_data.positions.size() * sizeof (QVector3D))); // allocate with writing
        }
        m_shader->setAttributeBuffer("vertex_position", GL_FLOAT, 0, 3);
        m_shader->enableAttributeArray("vertex_position");
        vbo_position->release();

        std::unique_ptr<QOpenGLBuffer> vbo_color = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
        vbo_color->create();
        vbo_color->bind();
        vbo_color->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (vertex_data.colors.size() == 0 || !m_use_original_colors){
            std::vector<QVector4D> colors = compute_colors_from_depth(vertex_data.positions, m_pc_encoding);
            vbo_color->allocate(colors.data(), static_cast<int>(colors.size() * sizeof (QVector4D))); // allocate with writing
        } else {
            vbo_color->allocate(vertex_data.colors.data(), static_cast<int>(vertex_data.colors.size() * sizeof (QVector4D))); // allocate with writing
        }
        m_shader->setAttributeBuffer("vertex_color", GL_FLOAT, 0, 4);
        m_shader->enableAttributeArray("vertex_color");
        vbo_color->release();

    m_vao->release();
    m_shader->release();

    m_initialized = true;
}
