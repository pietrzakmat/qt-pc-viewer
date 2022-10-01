#ifndef GLBASISOBJECT_H
#define GLBASISOBJECT_H

#include <iostream>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QColor>

#include <openglwindow.h>

class GLBasisObject
{
public:
    void initialize_gl();
    void draw();
    graphics::VertexData generate_vertex_data();

    void set_shader(QOpenGLShaderProgram* shader) {m_shader = shader;}
    QOpenGLShaderProgram* get_shader_program() {return m_shader;}

private:
    void initialize_vertices(const graphics::VertexData& vertices);

private:
    const GLsizei m_lines_vertex_count {6};
    const GLsizei m_triangles_count {12};
    bool m_gl_initialized {false};
    std::size_t m_vertices_count {0};

    std::unique_ptr<QOpenGLVertexArrayObject> m_vao {nullptr};
    QOpenGLShaderProgram* m_shader {nullptr};

};

#endif // GLBASISOBJECT_H
