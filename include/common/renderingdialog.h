#ifndef RENDERINGDIALOG_H
#define RENDERINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>

#include <camera.h>

class RenderingDialog : public QDialog
{
    Q_OBJECT
public:
    RenderingDialog(QWidget *parent = nullptr, Camera *camera=nullptr);

    QSize entry_size {QSize(50, 25)};

    // VIEW
    QGroupBox *m_view_control_box {nullptr};
    QGroupBox *m_view_matrix_box {nullptr};
    QGroupBox *m_draw_controls_box {nullptr};
    QWidget* create_view_widget();
    QGroupBox* create_view_matrix_box();

    std::vector<QLabel*> m_view_matrix_lbls;
    // VIEW
    QCheckBox *m_arcball_box {nullptr};

    // PROJECTION
    QGroupBox *m_projection_control_box {nullptr};
    QGroupBox *m_projection_matrix_box {nullptr};
    QGroupBox *m_perspective_proj_fov_box {nullptr};
    QGroupBox *m_ortho_proj_box {nullptr};

    QComboBox *m_view_model_type_cbx {nullptr};
    QWidget* create_projection_widget();
    QGroupBox* create_projection_control_box();
    QGroupBox* create_projection_matrix_box();
    QLabel* m_proj_fov_y_lbl {nullptr};
    QLabel* m_proj_aspect_ratio_lbl {nullptr};
    QSlider* m_proj_fov_y_sld {nullptr};

    std::vector<QLabel*> m_proj_matrix_lbls;
    // PROJECTION

public slots:
    void slot_process_universal_line_edit_finished();
    void slot_process_universal_slider_value_changed();
    void slot_process_universal_checkbox(bool checked);
    void slot_update_rendering_dialog();
    void slot_process_universal_combobox_index(int idx);

protected:
    void append_edits_to_row_in_layout(QFormLayout *formLayout, QWidget *lbl, const std::vector<QLineEdit*> &line_edits);
    void append_edits_to_row_in_layout(QFormLayout *formLayout, QLabel* lbl, const std::vector<QLineEdit*> &line_edits, QLabel* lbl_after);
    QWidget* append_edits_to_row_in_layout(QLabel* lbl, const std::vector<QLineEdit*> &line_edits);
    void append_labels_to_row_in_layout(QFormLayout *formLayout, const std::vector<QLabel*> &labels);

    // add to QFormLayout
    QWidget* create_custom_widget(QSlider *sld, QLabel *lbl);
    QWidget* create_custom_widget(QLabel *lbl1, QLabel *lbl2);

private:
    Camera* m_camera {nullptr};
};

//****** SLOTS *****//
inline
void RenderingDialog::slot_process_universal_combobox_index(int idx)
{
    if (nullptr == m_camera)
        return;

    QObject* obj = sender();

    if (obj == m_view_model_type_cbx){
        slot_update_rendering_dialog();
    }
}

inline
void RenderingDialog::slot_update_rendering_dialog()
{
    if (nullptr == m_camera)
        return;

    constexpr int precision = 2;
    const QMatrix4x4 view = m_camera->get_view_matrix();
    const QMatrix4x4 proj = m_camera->get_projection_matrix();

    m_proj_fov_y_lbl->setText(QString::number(static_cast<double>(m_camera->fov_y)));
    m_proj_aspect_ratio_lbl->setText(QString::number(static_cast<double>(m_camera->aspect_ratio),'f',precision));
    m_proj_fov_y_sld->setValue(static_cast<int>(m_camera->fov_y));

    // Update gui with the view matrix:
    for (int row = 0; row < 4; ++row) {
        std::size_t offset = static_cast<std::size_t>(row*4);
        m_view_matrix_lbls[offset+0]->setText(QString::number(static_cast<double>(view.row(row).x()),'f',3));
        m_view_matrix_lbls[offset+1]->setText(QString::number(static_cast<double>(view.row(row).y()),'f',3));
        m_view_matrix_lbls[offset+2]->setText(QString::number(static_cast<double>(view.row(row).z()),'f',3));
        m_view_matrix_lbls[offset+3]->setText(QString::number(static_cast<double>(view.row(row).w()),'f',3));
    }

    // Update gui with the projection matrix:
    for (int row = 0; row < 4; ++row) {
        std::size_t offset = static_cast<std::size_t>(row*4);
        m_proj_matrix_lbls[offset+0]->setText(QString::number(static_cast<double>(proj.row(row).x()),'f',3));
        m_proj_matrix_lbls[offset+1]->setText(QString::number(static_cast<double>(proj.row(row).y()),'f',3));
        m_proj_matrix_lbls[offset+2]->setText(QString::number(static_cast<double>(proj.row(row).z()),'f',3));
        m_proj_matrix_lbls[offset+3]->setText(QString::number(static_cast<double>(proj.row(row).w()),'f',3));
    }
}

inline
void RenderingDialog::slot_process_universal_checkbox(bool checked)
{
    if (nullptr == m_camera)
        return;
    QObject* obj = sender();

    if (obj == m_perspective_proj_fov_box){
        m_camera->m_projection_type = checked ? Camera::perspective : Camera::orthographic;
        m_ortho_proj_box->setChecked(!checked);
    } else if (obj == m_ortho_proj_box){
        m_camera->m_projection_type = checked ? Camera::orthographic : Camera::perspective;
        m_perspective_proj_fov_box->setChecked(!checked);
    }

    m_camera->update();
    slot_update_rendering_dialog();
}

inline
void RenderingDialog::slot_process_universal_slider_value_changed()
{
    if (nullptr == m_camera)
        return;
    const QObject* obj = sender();
    const int v = qobject_cast<QSlider*>(sender())->value();

    if (obj == m_proj_fov_y_sld) {
        m_camera->fov_y = static_cast<float>(v);
        m_proj_fov_y_lbl->setText(QString::number(static_cast<double>(m_camera->fov_y)).rightJustified(3, '0'));
        m_proj_aspect_ratio_lbl->setText(QString::number(static_cast<double>(m_camera->aspect_ratio),'f',2));

        m_camera->update();
        slot_update_rendering_dialog();  // to update projection matrix in gui
    }
}

inline
void RenderingDialog::slot_process_universal_line_edit_finished()
{
    if (nullptr == m_camera)
        return;
    QObject* obj = sender();
    if (qobject_cast<QLineEdit*>(obj)->text() == QString(""))
        qobject_cast<QLineEdit*>(obj)->setText("0");

    const float v = qobject_cast<QLineEdit*>(obj)->text().toFloat();

    m_camera->update();

    slot_update_rendering_dialog();  // to update view matrix in gui
}

//****** PROTECTED *****//
inline
void RenderingDialog::append_edits_to_row_in_layout(QFormLayout *formLayout, QWidget* lbl, const std::vector<QLineEdit*> &line_edits)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(lbl);
    for (std::size_t i=0; i<line_edits.size(); i++) {
        layout->addWidget(line_edits.at(i));
    }
    layout->addStretch();
    widget->setLayout(layout);
    formLayout->addRow(widget);
}

inline
void RenderingDialog::append_edits_to_row_in_layout(QFormLayout *formLayout, QLabel* lbl, const std::vector<QLineEdit*> &line_edits, QLabel* lbl_after)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(lbl);
    for (std::size_t i=0; i<line_edits.size(); i++) {
        layout->addWidget(line_edits.at(i));
    }
    layout->addWidget(lbl_after);
    layout->addStretch();
    widget->setLayout(layout);
    formLayout->addRow(widget);
}

inline
QWidget* RenderingDialog::append_edits_to_row_in_layout(QLabel* lbl, const std::vector<QLineEdit*> &line_edits)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(lbl);
    for (std::size_t i=0; i<line_edits.size(); i++) {
        layout->addWidget(line_edits.at(i));
    }
    layout->addStretch();
    widget->setLayout(layout);
    return widget;
}

inline
void RenderingDialog::append_labels_to_row_in_layout(QFormLayout *formLayout, const std::vector<QLabel*> &labels)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    for (std::size_t i=0; i<labels.size(); i++) {
        layout->addWidget(labels.at(i));
    }
    layout->addStretch();
    widget->setLayout(layout);
    formLayout->addRow(widget);
}

inline
QWidget* RenderingDialog::create_custom_widget(QSlider* sld, QLabel* lbl)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(sld);
    layout->addWidget(lbl);
    widget->setLayout(layout);
    return widget;
}

inline
QWidget* RenderingDialog::create_custom_widget(QLabel *lbl1, QLabel *lbl2)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(lbl1);
    layout->addWidget(lbl2);
    widget->setLayout(layout);
    return widget;
}

#endif // RENDERINGDIALOG_H
