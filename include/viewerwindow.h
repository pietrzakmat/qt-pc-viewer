#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <algorithm>
#include <thread>
#include <random>

#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QLineEdit>
#include <QLabel>

#include "openglwindow.h"
//#include "viewcontroller.h"
#include "camera.h"
#include "plyloader.h"
#include "glpointcloudobject.h"
#include "glpointobject.h"
#include "glbasisobject.h"
#include "glcameraobject.h"
#include "glgroundgridobject.h"

static const QColor red_color = QColor(255,0,0);
static const QColor green_color = QColor(0,255,0);
static const QColor blue_color = QColor(0,0,255);
static const QColor cyan_color = QColor(0,255,255);

class ViewerWindow : public OpenGLWindow
{
    Q_OBJECT
public:
    explicit ViewerWindow(std::shared_ptr<QOpenGLContext> opengl_context=nullptr, QWindow *parent=nullptr);

    std::shared_ptr<Camera> m_camera_gl {nullptr};

    std::unique_ptr<GLBasisObject> m_basis_center_object {};
    std::unique_ptr<GLPointCloudObject> m_pointcloud_object {};
    std::unique_ptr<GLCameraObject> m_camera_object {};

    std::unique_ptr<GLPointObject> m_center_point_object {}; // focal_point
    std::unique_ptr<GLGroundGridObject> m_ground_grid_object {};

    void open_ply(const std::string& fname);
    bool m_update_pointcloud {false};
    std::string m_path_file;

    std::unique_ptr<QOpenGLShaderProgram> m_shader;
    std::unique_ptr<QOpenGLShaderProgram> create_drawing_shader();

    bool m_is_left_mouse_pressed {false};
    bool m_is_right_mouse_pressed {false};
    float m_camera_height {1.0F}; // in meters. Used to place the ground grid properly.
    bool m_draw_center_frame{true};

signals:
    void sig_update();

private:
    graphics::VertexData m_point_cloud_vertex_data;

protected:
    void initialize_gl() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void paint(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
};

#endif // VIEWERWINDOW_H
