#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <memory>

#include <QMatrix4x4>
#include <QVector2D>
#include <QOpenGLShaderProgram>

#include "graphics_math.hpp"

class Camera
{
public:
    Camera(const QVector3D& eye, const QVector3D& center, const QVector3D& up=QVector3D(0, 1, 0))
        : m_eye(eye)
        , m_center(center)
    {
        const QVector3D dir = center - eye;
        QVector3D z_axis = dir.normalized();
        QVector3D x_axis = QVector3D::crossProduct(z_axis, up.normalized()).normalized();
        QVector3D y_axis = QVector3D::crossProduct(x_axis, z_axis).normalized();
        x_axis = QVector3D::crossProduct(z_axis, y_axis).normalized();

        m_orientation_quat.fromAxes(x_axis, y_axis, -z_axis);

        m_center_translation.setToIdentity();
        m_center_translation.translate(center);
        m_center_translation = m_center_translation.inverted();

        m_translation_matrix.setToIdentity();
        m_translation_matrix.translate(QVector3D(0.f, 0.f, -dir.length()));
    }

    QVector2D prev_mouse {};
    QVector2D transform_mouse(const float x, const float y);
    // Project the point in [-1, 1] screen space onto the arcball sphere
    QQuaternion screen_to_arcball(const QVector2D &p);
    /* Rotate the camera from the previous mouse position to the current
     * one. Mouse positions should be in normalized device coordinates
     */
    void rotate(QVector2D prev_mouse, QVector2D cur_mouse);

    /* Pan the camera given the translation vector. Mouse
     * delta amount should be in normalized device coordinates
     */
    void pan(QVector2D mouse_delta);

    /* Zoom the camera given the zoom amount to (i.e., the scroll/wheel amount).
     * Positive values zoom in, negative will zoom out.
     */
    void zoom(const float zoom_amount);

    // Get the eye direction of the camera in world space
    QVector3D dir() const;

    static constexpr float zoom_coefficient_qt = 0.001f;
    void update();

    QMatrix4x4 get_view_matrix() const;
    QMatrix4x4 get_projection_matrix() const;
    QMatrix4x4 get_projection_view_matrix() const;

    void set_standard_uniforms(QOpenGLShaderProgram* shader, const QMatrix4x4 &model_matrix) const;
    void set_window_size(const std::size_t width, const std::size_t height);
    QVector2D window_size() const;

    QVector3D m_eye    {QVector3D(0.0f, 0.0f, 0.0f)};
    QVector3D m_center {QVector3D(0.0f, 0.0f, 0.0f)};

    mutable QMatrix4x4 m_proj_matrix;
    mutable QMatrix4x4 m_view_matrix;
    // camera is the full camera transform,
    // m_inv_camera_matrix is stored as well to easily compute
    // eye position and world space rotation axes
    QMatrix4x4 m_inv_camera_matrix;
    std::size_t m_window_width {1};
    std::size_t m_window_height {1};
    // Unmodified "look at" matrix (as the full camera transform) along with
    // decomposed translation and rotation components
    QMatrix4x4 m_center_translation;
    QMatrix4x4 m_translation_matrix;
    QQuaternion m_orientation_quat;

    // Projection
    enum projection_type {
        perspective,
        orthographic
    };
    projection_type m_projection_type {perspective};
    void calc_projection_matrix();
    static constexpr float fov_max = 180.0f;
    static constexpr float fov_min = 1.0F;
    float fov_y {45.f};
    float aspect_ratio = 1.f; // width/height
    float near_clipping_plane {0.01f};
    float far_clipping_plane {1000.0f};
};

inline QQuaternion
Camera::screen_to_arcball(const QVector2D &p)
{
    const float dist = QVector2D::dotProduct(p, p);
    // If we are on/in the sphere return the point on it
    if (dist <= 1.f) {
        return QQuaternion(0.0, p.x(), p.y(), std::sqrt(1.f - dist));
    } else {
        // otherwise we project the point onto the sphere
        const QVector2D proj = p.normalized();
        return QQuaternion(0.0, proj.x(), proj.y(), 0.f);
    }
}

inline void
Camera::rotate(QVector2D prev_mouse, QVector2D cur_mouse)
{
    // Clamp mouse positions to stay in NDC
    prev_mouse.setX(graphics::math::clampf(prev_mouse.x(), -1.f, 1.f));
    prev_mouse.setY(graphics::math::clampf(prev_mouse.y(), -1.f, 1.f));
    cur_mouse.setX(graphics::math::clampf(cur_mouse.x(), -1.f, 1.f));
    cur_mouse.setY(graphics::math::clampf(cur_mouse.y(), -1.f, 1.f));

    const QQuaternion mouse_cur_ball = screen_to_arcball(cur_mouse);
    const QQuaternion mouse_prev_ball = screen_to_arcball(prev_mouse);

    m_orientation_quat = mouse_cur_ball * mouse_prev_ball * m_orientation_quat;
    update();
}

inline void
Camera::pan(QVector2D mouse_delta)
{
    const float zoom_amount = std::abs(m_translation_matrix(2,3)); // z
    QVector4D motion(mouse_delta.x() * zoom_amount, mouse_delta.y() * zoom_amount, 0.f, 0.f);
    // Find the panning amount in the world space
    motion = m_view_matrix.inverted() * motion;

    QMatrix4x4 m;
    m.translate(QVector3D(motion));

    m_center_translation = m * m_center_translation;
    update();
}

inline void
Camera::zoom(const float zoom_amount)
{
    const QVector3D motion(0.f, 0.f, zoom_amount*zoom_coefficient_qt);
    QMatrix4x4 m;
    m.translate(QVector3D(motion));

    m_translation_matrix = m * m_translation_matrix;
    update();
}

inline QVector3D
Camera::dir() const
{
    return QVector3D(m_view_matrix.inverted() * QVector4D(0, 0, -1, 0)).normalized();
}

inline QVector2D
Camera::transform_mouse(const float x, const float y)
{
   return QVector2D((x * 2.0F / window_size().x()) - 1.0F, 1.0F - (2.0F * y / window_size().y()));
}

inline void
Camera::update()
{
    calc_projection_matrix();

    m_view_matrix.setToIdentity();
    m_view_matrix = m_translation_matrix * QMatrix4x4(m_orientation_quat.toRotationMatrix()) * m_center_translation;
    m_inv_camera_matrix = m_view_matrix.inverted();
}


inline void
Camera::calc_projection_matrix()
{
    m_proj_matrix.setToIdentity();
    if (Camera::perspective == m_projection_type){
        aspect_ratio = static_cast<float>(m_window_width) / static_cast<float>(m_window_height);
        m_proj_matrix.perspective(fov_y, aspect_ratio, near_clipping_plane, far_clipping_plane);

//        float left   = -near_clipping_plane;
//        float right  =  near_clipping_plane;
//        float bottom = -near_clipping_plane;
//        float top    =  near_clipping_plane;
//        m_proj_matrix.frustum(left, right, bottom, top, near_clipping_plane, far_clipping_plane);
    } else if (Camera::orthographic == m_projection_type){
        float aspect = 1.0F; // static_cast<float>(m_window_width) / static_cast<float>(m_window_height);
        float zoom_level = 1.f;
        const float l = -zoom_level*aspect;
        const float r = +zoom_level*aspect;
        const float b = -zoom_level*aspect;
        const float t = +zoom_level*aspect;
        m_proj_matrix.ortho(l, r, b, t, -5.f, 5.f);
    }
}

inline QMatrix4x4
Camera::get_view_matrix() const
{
    return m_view_matrix;
}

inline QMatrix4x4
Camera::get_projection_matrix() const
{
    return m_proj_matrix;
}

inline QMatrix4x4
Camera::get_projection_view_matrix() const
{
    return m_proj_matrix * m_view_matrix;
}

inline void
Camera::set_standard_uniforms(QOpenGLShaderProgram* shader, const QMatrix4x4& model_matrix) const
{
    if (!shader)
        return;

    QMatrix4x4 model_view_matrix = m_view_matrix * model_matrix;
    QMatrix4x4 mvp = m_proj_matrix * model_view_matrix;

    shader->bind();
    shader->setUniformValue("model_matrix", model_matrix);
    shader->setUniformValue("model_view_matrix", model_view_matrix);
    shader->setUniformValue("projection_matrix", m_proj_matrix);
    shader->setUniformValue("mvp", mvp);
    shader->release();
}

inline void
Camera::set_window_size(const std::size_t width, const std::size_t height)
{
    m_window_width = width;
    m_window_height = height;
}

inline QVector2D
Camera::window_size() const
{
    return QVector2D(static_cast<float>(m_window_width), static_cast<float>(m_window_height));
}



//inline void
//Camera::calculate_translation()
//{
//    m_center_translation.setToIdentity();
//    m_center_translation.translate(m_center);
//    m_center_translation = m_center_translation.inverted();

//    m_translation_matrix.setToIdentity();
//    m_translation_matrix.translate(QVector3D(0.f, 0.f, -direction().length()));
//}

//inline void
//Camera::calculate_rotation()
//{
//    QVector3D z_axis = direction().normalized();
//    QVector3D x_axis = QVector3D::crossProduct(z_axis, m_up.normalized()).normalized();
//    QVector3D y_axis = QVector3D::crossProduct(x_axis, z_axis).normalized();
//    x_axis = QVector3D::crossProduct(z_axis, y_axis).normalized();

//    m_orientation_quat.fromAxes(x_axis, y_axis, -z_axis);
//}

//inline QVector3D
//Camera::direction() const
//{
//    return m_center - m_eye;
//}


#endif // CAMERA_H
