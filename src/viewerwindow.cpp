#include "viewerwindow.h"

ViewerWindow::ViewerWindow(std::shared_ptr<QOpenGLContext> opengl_context, QWindow *parent)
    : OpenGLWindow(opengl_context, parent)
{
    QVector3D eye(0.0f, 0.0f, 1.0F);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D up(0.0f, 1.0F, 0.0f);
    m_camera_gl = std::make_shared<Camera>(eye, center, up);
}

void ViewerWindow::open_ply(const std::string& fname)
{
    m_point_cloud_vertex_data = graphics::read_ply(fname);
    m_update_pointcloud = true;

//    std::string test_name = fname;
//    test_name.replace(test_name.size()-4, test_name.size(), "");
//    test_name+="test.ply";
//    std::cerr << "test_name: " << test_name << "\n";
//    graphics::write_ply(test_name, m_vertex_data);
}

std::unique_ptr<QOpenGLShaderProgram> ViewerWindow::create_drawing_shader()
{
    std::unique_ptr<QOpenGLShaderProgram> shader = std::make_unique<QOpenGLShaderProgram>();
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             "#version 130\n"
                                             "in vec3 vertex_position;\n"
                                             "in vec4 vertex_color;\n"
                                             "in vec4 main_color;\n"
                                             "out vec4 color;\n"
                                             "uniform mat4 mvp;\n"
                                             "void main(void)\n"
                                             "{\n"
                                             "    gl_Position = mvp * vec4(vertex_position, 1.0F);\n"
                                             "    color = vertex_color * main_color;\n"
                                             "}\n"
                                         );
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             "#version 130\n"
                                             "in vec4 color;\n"
                                             "out vec4 frag_color;\n"
                                             "void main(void)\n"
                                             "{\n"
                                             "    frag_color = color;\n"
                                             "}\n"
                                         );
    shader->link();

    if (!shader->isLinked()) {
        std::cerr << "Error: m_shader_t shader is not linked" << std::endl;
    }

    return shader;
}

void ViewerWindow::initialize_gl()
{
    m_shader = create_drawing_shader();

    m_basis_center_object = std::make_unique<GLBasisObject>();
    m_basis_center_object->set_shader(m_shader.get());
    m_basis_center_object->initialize_gl();

    m_camera_object = std::make_unique<GLCameraObject>();
    m_camera_object->set_shader(m_shader.get());
    m_camera_object->initialize_gl();

    m_pointcloud_object = std::make_unique<GLPointCloudObject>();
    m_pointcloud_object->set_shader(m_shader.get());
    m_pointcloud_object->initialize_gl();

    m_center_point_object = std::make_unique<GLPointObject>();
    m_center_point_object->set_shader(m_shader.get());
    m_center_point_object->initialize_gl();
    m_center_point_object->set_size(15);

    m_ground_grid_object = std::make_unique<GLGroundGridObject>();
    m_ground_grid_object->set_shader(m_shader.get());
    m_ground_grid_object->initialize_gl();
}

void ViewerWindow::resizeGL(int width, int height)
{
//    std::cerr << __PRETTY_FUNCTION__ << std::endl;
    glViewport(0, 0, width, height);

    if (nullptr != m_camera_gl)
        m_camera_gl->set_window_size(static_cast<std::size_t>(width), static_cast<std::size_t>(height));

    m_camera_gl->update();
}

void ViewerWindow::paintGL()
{
    if (nullptr == m_camera_gl || nullptr == m_pointcloud_object)
        return;

//    using namespace std::chrono_literals;
//    std::this_thread::sleep_for(20ms);

    if (m_update_pointcloud) {
        m_update_pointcloud = false;
        m_pointcloud_object->set_points(m_point_cloud_vertex_data);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_camera_gl->set_standard_uniforms(m_pointcloud_object->get_shader_program(), QMatrix4x4());
    m_pointcloud_object->draw(m_pointcloud_object->m_point_size);

    if (m_ground_grid_object) {
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        QMatrix4x4 model;
        model.translate(QVector3D(0,-m_camera_height,0));
        model.scale(10);
        model.rotate(90, QVector3D(1,0,0));
        m_camera_gl->set_standard_uniforms(m_ground_grid_object->get_shader_program(), model);
        m_ground_grid_object->draw();
        glDisable(GL_BLEND);
    }

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // for transparent vertex color

    if (m_draw_center_frame) {

        if (m_basis_center_object) {
            QMatrix4x4 model;
            model.translate(m_camera_gl->m_center);
            model.scale(0.2f);
            model.rotate(180, QVector3D(1,0,0));

            m_camera_gl->set_standard_uniforms(m_basis_center_object->get_shader_program(), model);
            m_basis_center_object->draw();
        }
        if (m_center_point_object) {
            graphics::VertexData center_of_view;
            center_of_view.positions.emplace_back(m_camera_gl->m_center);
            center_of_view.colors.emplace_back(QVector4D(1,1,0,0.5f));
            m_center_point_object->set_point(center_of_view);

            m_camera_gl->set_standard_uniforms(m_center_point_object->get_shader_program(), QMatrix4x4());
            m_center_point_object->draw();
        }

        if (m_camera_object) {
            QMatrix4x4 model;
            model.translate(m_camera_gl->m_center);
            model.scale(0.1f);
            model.rotate(180, QVector3D(1,0,0));

            m_camera_gl->set_standard_uniforms(m_camera_object->get_shader_program(), model);
            m_camera_object->draw(QColor(0, 255, 255));
        }
    }

}

void ViewerWindow::paint(QPainter &painter)
{

}

void ViewerWindow::keyPressEvent(QKeyEvent *e)
{
    Q_EMIT sig_update();
}

void ViewerWindow::keyReleaseEvent(QKeyEvent *e)
{
}

void ViewerWindow::mousePressEvent(QMouseEvent *e)
{
    m_is_left_mouse_pressed = (e->button() == Qt::LeftButton);
    m_is_right_mouse_pressed = (e->button() == Qt::RightButton);
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_is_left_mouse_pressed = false;
    if (e->button() == Qt::RightButton)
        m_is_right_mouse_pressed = false;
}

void ViewerWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (nullptr == m_camera_gl)
        return;

    const QVector2D cur_mouse = m_camera_gl->transform_mouse(e->x(), e->y());

    if (m_is_left_mouse_pressed) {
        m_camera_gl->rotate(m_camera_gl->prev_mouse, cur_mouse);
    } else if (m_is_right_mouse_pressed) {
        m_camera_gl->pan(cur_mouse - m_camera_gl->prev_mouse);
    }

    m_camera_gl->prev_mouse = cur_mouse;

    if (m_is_left_mouse_pressed || m_is_right_mouse_pressed)
        Q_EMIT sig_update();
}

void ViewerWindow::wheelEvent(QWheelEvent *e)
{
    m_camera_gl->zoom((-1)*static_cast<float>(e->angleDelta().y()));
    Q_EMIT sig_update();
}
