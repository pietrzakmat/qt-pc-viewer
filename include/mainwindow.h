#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>

#include "viewerwindow.h"
#include "renderingdialog.h"
#include "pointcloudcontroldialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void create_menu_bar();
    void create_rendering_dialog();
    void create_pc_control_dialog();
    void create_about_dialog();

public slots:
    void open_file_dialog();
    void open_view(const QString& ply_path);
    void close_view();

private:
    void update_stats();
    void reset_camera_view();

private:
    const QString m_title = QObject::tr("Point Cloud Viewer");
    std::unique_ptr<QTimer> m_update_timer {nullptr};
    std::unique_ptr<ViewerWindow> m_gl_window {nullptr};
    RenderingDialog* m_rendering_dialog {nullptr};
    PointControlDialog* m_plycontrol_dialog {nullptr};
    QMessageBox* m_about_dialog {nullptr};
};

inline
void MainWindow::reset_camera_view()
{
    if (nullptr == m_gl_window || nullptr == m_gl_window->m_camera_gl) {
        return;
    }


//    m_gl_window->m_camera->reset_view();
//    m_gl_window->m_camera->fov_y = 45.0f;
    if (m_rendering_dialog){
        m_rendering_dialog->slot_update_rendering_dialog();
    }
}

inline
void MainWindow::update_stats()
{
    if(!m_gl_window)
        return;

    setWindowTitle(QString("%1 - %2").arg(QString::fromStdString(m_gl_window->m_path_file)).arg(m_title)
                   + " | FPS: " + QString::number(m_gl_window->fps(), 'f', 1)
                   + " ("  + QString::number(m_gl_window->render_time(), 'f', 1)+ " ms)");
}

inline void MainWindow::create_about_dialog()
{
    QString t =  tr("Qt Point Cloud Viewer");
    t.append("\nCopyright (c) 2022 Mateusz Pietrzak (MIT License)");
    t.append("\n\nBuild with Qt version: ");
    t.append(qVersion());

    t.append("\n\nAcknowledgements:");
    t.append("\n1. Tinycolormap. Author: Yuki Koyama (2018, MIT)");
    t.append("\n2. Tinyply. Author: Dimitri Diakopoulos (public domain)");

    QMessageBox::about(this, "About Qt-pc-viewer", t);
}

inline
void MainWindow::create_rendering_dialog()
{
    if (nullptr == m_gl_window || nullptr == m_gl_window->m_camera_gl) {
        qDebug() << Q_FUNC_INFO << "Error: m_gl_window=" << m_gl_window.get() << "m_gl_window->m_gl_camera" << m_gl_window->m_camera_gl.get();
        return;
    }

    if (nullptr == m_rendering_dialog)
        m_rendering_dialog = new RenderingDialog(this, m_gl_window->m_camera_gl.get());
    connect(m_gl_window.get(), &ViewerWindow::sig_update, m_rendering_dialog, &RenderingDialog::slot_update_rendering_dialog);

    m_rendering_dialog->show();
    m_rendering_dialog->slot_update_rendering_dialog();
}

inline
void MainWindow::create_pc_control_dialog()
{
    if (nullptr == m_gl_window)
        return;

    if (nullptr == m_plycontrol_dialog)
        m_plycontrol_dialog = new PointControlDialog(this, m_gl_window.get());

    m_plycontrol_dialog->show();
}

#endif // MainWindow_H
