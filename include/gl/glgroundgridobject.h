#ifndef GLGROUNDGRIDOBJECT_H
#define GLGROUNDGRIDOBJECT_H

#include <iostream>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QColor>

#include <openglwindow.h>

class GLGroundGridObject
{
public:
    void initialize_gl();
    void draw(const QColor& color = QColor(255,255,255));
    graphics::VertexData generate_vertex_data();

    void set_shader(QOpenGLShaderProgram* shader) {m_shader = shader;}
    QOpenGLShaderProgram* get_shader_program() {return m_shader;}

private:
    void initialize_vertices(const graphics::VertexData& vertices);

private:
    bool m_gl_initialized {false};
    std::size_t m_vertices_count {0};
    // Buffers
    std::unique_ptr<QOpenGLVertexArrayObject> m_vao {nullptr};
    // Shaders
    QOpenGLShaderProgram* m_shader {nullptr};
};

#endif // GLGROUNDGRIDOBJECT_H
