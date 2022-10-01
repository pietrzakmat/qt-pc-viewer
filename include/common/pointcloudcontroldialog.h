#ifndef POINTCLOUDCONTROLDIALOG_H
#define POINTCLOUDCONTROLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>

#include <viewerwindow.h>
#include <glpointcloudobject.h>

class PointControlDialog : public QDialog
{
    Q_OBJECT
public:
    PointControlDialog(QWidget *parent = nullptr, ViewerWindow *gl_window=nullptr)
        : QDialog(parent)
        , m_viewer_window(gl_window)
    {
        setWindowTitle("Point Cloud Control");
        setWindowFlags(Qt::Dialog);

        setModal(false);
        setAttribute(Qt::WA_ShowModal, false); // Set the WA_ShowModal property
        setWindowModality(Qt::NonModal);

        constexpr int window_width = 250;
        constexpr int window_height = 250;
        resize(window_width, window_height);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setSizePolicy(sizePolicy);
        setMinimumSize(QSize(window_width, window_height));
        setMaximumSize(QSize(window_width, window_height));

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(create_pc_control_widget());

        connect(m_rgb_original_box, &QGroupBox::clicked, this, &PointControlDialog::slot_process_universal_checkbox);
        connect(m_rgb_encoding_box,  &QGroupBox::clicked, this, &PointControlDialog::slot_process_universal_checkbox);
        connect(m_lut_cbx, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointControlDialog::slot_process_combo_box_changed);
        connect(m_lut_inversion_chbx, &QCheckBox::stateChanged, this, &PointControlDialog::slot_process_universal_checkbox);
        connect(m_point_size_sld, &QSlider::valueChanged, this, &PointControlDialog::slot_process_universal_slider_value_changed);

        setLayout(mainLayout);
    }

    QWidget* create_pc_control_widget()
    {
        m_point_size_widget = create_point_size_widget();
        m_rgb_original_box = create_rgb_original_control_box(); // needs the member of the class to live after scope
        m_rgb_encoding_box = create_rgb_encoding_control_box(); // needs the member of the class to live after scope

        QVBoxLayout *vertical_layout = new QVBoxLayout;
        vertical_layout->addWidget(m_point_size_widget);
        vertical_layout->addWidget(m_rgb_original_box);
        vertical_layout->addWidget(m_rgb_encoding_box);
        vertical_layout->addStretch();
        QWidget *widget  = new QWidget;
        widget->setLayout(vertical_layout);

        return widget;
    }

    QWidget* create_point_size_widget();
    QGroupBox* create_rgb_original_control_box();
    QGroupBox* create_rgb_encoding_control_box();

    QGroupBox *m_rgb_original_box {nullptr};
    QGroupBox *m_rgb_encoding_box {nullptr};
    QComboBox* m_lut_cbx {nullptr};
    QCheckBox* m_lut_inversion_chbx {nullptr};
    QWidget* m_point_size_widget {nullptr};
    QLabel* m_point_size_lbl {nullptr};
    QSlider* m_point_size_sld {nullptr};

    ViewerWindow* m_viewer_window {nullptr};
    void set_gl_window(ViewerWindow* window) { m_viewer_window = window; }

public slots:
    void slot_process_universal_checkbox(bool checked);
    void slot_process_combo_box_changed(int v);
    void slot_process_universal_slider_value_changed();
};

inline
QWidget* PointControlDialog::create_point_size_widget()
{
    if (nullptr == m_viewer_window || nullptr == m_viewer_window->m_pointcloud_object)
        return new QWidget;

    QWidget* w = new QWidget;
    QString str = QString("Point size: %1").arg(QString::number(m_viewer_window->m_pointcloud_object->m_point_size));

    m_point_size_lbl = new QLabel(str);
    m_point_size_sld = new QSlider(Qt::Horizontal);
    m_point_size_sld->setRange(1, 10);
    m_point_size_sld->setTickInterval(1);
    m_point_size_sld->setValue(static_cast<int>(m_viewer_window->m_pointcloud_object->m_point_size));

    QFormLayout *form_layout = new QFormLayout;
    form_layout->addRow(m_point_size_lbl);
    form_layout->addRow(m_point_size_sld);
    w->setLayout(form_layout);

    return w;
}

inline
QGroupBox* PointControlDialog::create_rgb_original_control_box()
{
    QGroupBox* file_rgb_box = new QGroupBox(tr("Original RGB"));
    file_rgb_box->setCheckable(true);
    file_rgb_box->setChecked(true);

    return file_rgb_box;
}

inline
QGroupBox* PointControlDialog::create_rgb_encoding_control_box()
{
    QGroupBox* color_box = new QGroupBox(tr("Color Encoding"));
    color_box->setCheckable(true);
    color_box->setChecked(!m_viewer_window->m_pointcloud_object->m_use_original_colors);

    m_lut_inversion_chbx = new QCheckBox("Inverse depth");
    m_lut_inversion_chbx->setCheckable(true);
    m_lut_inversion_chbx->setChecked(m_viewer_window->m_pointcloud_object->m_inverse_depth_colors);

    m_lut_cbx = new QComboBox;
    m_lut_cbx->addItem("Depth grayscale");
    m_lut_cbx->addItem("LUT Turbo");
    m_lut_cbx->addItem("LUT Jet");
    m_lut_cbx->addItem("LUT Heat");

    if (m_viewer_window && m_viewer_window->m_pointcloud_object)
        m_lut_cbx->setCurrentIndex(m_viewer_window->m_pointcloud_object->m_pc_encoding);

    QFormLayout *form_layout = new QFormLayout;
    form_layout->addRow(m_lut_inversion_chbx);
    form_layout->addRow(m_lut_cbx);
    color_box->setLayout(form_layout);

    return color_box;
}

inline
void PointControlDialog::slot_process_universal_slider_value_changed()
{
    if (!m_viewer_window || !m_viewer_window->m_pointcloud_object)
        return;
    const QObject* obj = sender();
    const int v = qobject_cast<QSlider*>(sender())->value();

    if (obj == m_point_size_sld) {
        m_viewer_window->m_pointcloud_object->m_point_size = v;
        m_point_size_lbl->setText(QString("Point size: %1").arg(QString::number(m_viewer_window->m_pointcloud_object->m_point_size)));
    }
}


inline
void PointControlDialog::slot_process_universal_checkbox(bool checked)
{
    if (!m_viewer_window || !m_viewer_window->m_pointcloud_object)
        return;
    const QObject* obj = sender();

    if (obj == m_rgb_original_box){
        m_rgb_encoding_box->setChecked(!checked);
        m_viewer_window->m_pointcloud_object->m_use_original_colors = true;
        m_viewer_window->m_update_pointcloud = true;
    } else if (obj == m_rgb_encoding_box){
        m_rgb_original_box->setChecked(!checked);
        m_viewer_window->m_pointcloud_object->m_use_original_colors = false;
        m_viewer_window->m_update_pointcloud = true;
    } else if (obj == m_lut_inversion_chbx){
        m_lut_inversion_chbx->setChecked(checked);
        m_viewer_window->m_pointcloud_object->m_inverse_depth_colors = checked;
        m_viewer_window->m_update_pointcloud = true;
    }
}

inline
void PointControlDialog::slot_process_combo_box_changed(int v)
{
    if (!m_viewer_window || !m_viewer_window->m_pointcloud_object)
        return;

    if(static_cast<GLPointCloudObject::pc_encoding>(v) != m_viewer_window->m_pointcloud_object->m_pc_encoding){
        m_viewer_window->m_pointcloud_object->m_pc_encoding = static_cast<GLPointCloudObject::pc_encoding>(v);
        m_viewer_window->m_update_pointcloud = true;
    }
}


#endif // POINTCLOUDCONTROLDIALOG_H
