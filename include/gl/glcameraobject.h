#ifndef GLCAMERAOBJECT_H
#define GLCAMERAOBJECT_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QColor>

#include <openglwindow.h>

class GLCameraObject
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
    const GLint m_frame1 {0};
    const GLint m_frame2 {4};
    const GLint m_frame3 {7};
    bool m_gl_initialized {false};

    std::unique_ptr<QOpenGLVertexArrayObject> m_vao {nullptr};
    QOpenGLShaderProgram* m_shader {nullptr};
};

#endif // GLCAMERAOBJECT_H
