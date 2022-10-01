#include "renderingdialog.h"

RenderingDialog::RenderingDialog(QWidget *parent, Camera *camera)
    : QDialog(parent)
    , m_camera(camera)
{
    setWindowTitle("Rendering Camera Settings");
    setWindowFlags(Qt::Dialog);

    setModal(false);
    setAttribute(Qt::WA_ShowModal, false); // Set the WA_ShowModal property
    setWindowModality(Qt::NonModal);

    constexpr int window_width = 500;
    constexpr int window_height = 600;
    resize(window_width, window_height);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(window_width, window_height));
    setMaximumSize(QSize(window_width, window_height));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(create_view_widget());
    mainLayout->addWidget(create_projection_widget());

    setLayout(mainLayout);
}

QWidget* RenderingDialog::create_view_widget()
{
//    m_view_control_box = create_view_control_box(); // needs the member of the class to live after scope
    m_view_matrix_box = create_view_matrix_box(); // needs the member of the class to live after scope

    QHBoxLayout *horizontal_layout = new QHBoxLayout;
//    horizontal_layout->addWidget(m_view_control_box);
    horizontal_layout->addWidget(m_view_matrix_box);
    QWidget *view_control_widget  = new QWidget;
    view_control_widget->setLayout(horizontal_layout);


    return view_control_widget;
}

QGroupBox* RenderingDialog::create_view_matrix_box()
{
    m_view_matrix_lbls.reserve(16); // 4x4 matrix
    QFormLayout *view_matrix_form_layout = new QFormLayout;

    for (std::size_t i=0; i<4; i++) {
        QWidget* w = new QWidget(this);
        QHBoxLayout* l = new QHBoxLayout(this);
        std::size_t offset = static_cast<std::size_t>(i*4);
        for (std::size_t j=0; j<4; j++) {
            QLabel* label = new QLabel;
            label->setFixedSize(entry_size);
            label->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
            m_view_matrix_lbls.emplace_back(label);
            l->addWidget(m_view_matrix_lbls.at(offset+j));
        }
        l->addStretch();
        w->setLayout(l);
        view_matrix_form_layout->addRow(w);
    }
    QGroupBox* view_matrix_box = new QGroupBox(tr("View matrix:"));
    view_matrix_box->setLayout(view_matrix_form_layout);

    return view_matrix_box;
}

QWidget* RenderingDialog::create_projection_widget()
{
    m_projection_control_box = create_projection_control_box();
    m_projection_matrix_box = create_projection_matrix_box();
    QHBoxLayout *horizontal_layout = new QHBoxLayout;
    horizontal_layout->addWidget(m_projection_control_box);
    horizontal_layout->addWidget(m_projection_matrix_box);
    QWidget *proj_control_widget  = new QWidget;
    proj_control_widget->setLayout(horizontal_layout);

    return proj_control_widget;
}

QGroupBox* RenderingDialog::create_projection_control_box()
{
    if (nullptr == m_camera)
        return new QGroupBox;
    QFormLayout *projections_form_layout = new QFormLayout;
    m_perspective_proj_fov_box = new QGroupBox(tr("Perspective projection"));
    m_perspective_proj_fov_box->setCheckable(true);
    m_perspective_proj_fov_box->setChecked(m_camera->m_projection_type == Camera::perspective);
    m_proj_fov_y_lbl = new QLabel;
    m_proj_aspect_ratio_lbl = new QLabel;
    m_proj_aspect_ratio_lbl->setToolTip("Window width/height aspect ratio active");
    QLabel* ascpect_ratio_description_lbl = new QLabel("Aspect ratio horizontal FOV:");
    ascpect_ratio_description_lbl->setToolTip("Window width/height aspect ratio active");

    QFormLayout *proj_perspective_layout = new QFormLayout;
    m_proj_fov_y_sld = new QSlider(Qt::Horizontal);

    m_proj_fov_y_sld->setRange(static_cast<int>(m_camera->fov_min),
                                      static_cast<int>(m_camera->fov_max));

    proj_perspective_layout->addRow(new QLabel("Vertical FOV:"));
    proj_perspective_layout->addRow(create_custom_widget(m_proj_fov_y_sld, m_proj_fov_y_lbl));
    proj_perspective_layout->addRow(create_custom_widget(ascpect_ratio_description_lbl, m_proj_aspect_ratio_lbl));

    m_perspective_proj_fov_box->setLayout(proj_perspective_layout);

    m_ortho_proj_box = new QGroupBox(tr("Orthographic projection"));
    m_ortho_proj_box->setCheckable(true);
    m_ortho_proj_box->setChecked(m_camera->m_projection_type == Camera::orthographic);

    projections_form_layout->addRow(m_perspective_proj_fov_box);
    projections_form_layout->addRow(m_ortho_proj_box);

    // Connections
    connect(m_perspective_proj_fov_box, &QGroupBox::clicked,    this, &RenderingDialog::slot_process_universal_checkbox);
    connect(m_ortho_proj_box,           &QGroupBox::clicked,    this, &RenderingDialog::slot_process_universal_checkbox);
    connect(m_proj_fov_y_sld,    &QSlider::valueChanged, this, &RenderingDialog::slot_process_universal_slider_value_changed);

    QGroupBox* proj_matrix_box = new QGroupBox(tr("Projection Control:"));
    proj_matrix_box->setLayout(projections_form_layout);

    return proj_matrix_box;
}

QGroupBox* RenderingDialog::create_projection_matrix_box()
{
    m_proj_matrix_lbls.reserve(16); // 4x4 matrix
    QFormLayout *proj_matrix_form_layout = new QFormLayout;

    for (std::size_t i=0; i<4; i++) {
        QWidget* w = new QWidget(this);
        QHBoxLayout* l = new QHBoxLayout(this);
        std::size_t offset = static_cast<std::size_t>(i*4);
        for (std::size_t j=0; j<4; j++) {
            QLabel* label = new QLabel;
            label->setFixedSize(entry_size);
            label->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
            m_proj_matrix_lbls.emplace_back(label);
            l->addWidget(m_proj_matrix_lbls.at(offset+j));
        }
        l->addStretch();
        w->setLayout(l);
        proj_matrix_form_layout->addRow(w);
    }
    QGroupBox* proj_matrix_box = new QGroupBox(tr("Projection matrix:"));
    proj_matrix_box->setLayout(proj_matrix_form_layout);

    return proj_matrix_box;
}
