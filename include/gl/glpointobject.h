#ifndef GLPOINTOBJECT_H
#define GLPOINTOBJECT_H

#include <memory>
#include <mutex>
#include <iostream>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QVector3D>

#include <openglwindow.h>

class GLPointObject
{
public:
    void initialize_gl();
    void draw();

    void set_size(const float &size);
    void set_point(const graphics::VertexData &vertex_data);

    void set_shader(QOpenGLShaderProgram* shader) {m_shader = shader;}
    QOpenGLShaderProgram* get_shader_program() {return m_shader;}

private:
    bool m_gl_initialized {false};
    float m_point_size {1.f};

    std::unique_ptr<QOpenGLVertexArrayObject> m_vao {nullptr};
    QOpenGLShaderProgram* m_shader {nullptr};
};

inline
void GLPointObject::set_size(const float& size)
{
    m_point_size = size;
}

#endif // GLPOINTOBJECT_H
