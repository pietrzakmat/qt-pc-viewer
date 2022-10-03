#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>

#include <vector>
//#include <ranges>
#include <random>
#include <iostream>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    // fit into 80% of a desktop size
    const QRect desk = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(this));
    resize(static_cast<int>(desk.width() * .8f), static_cast<int>(desk.height() * .8f));

    setWindowTitle(m_title);
    setMinimumSize(600, 400);

    create_menu_bar();

    // take first command line argument as a path to PLY files
    if (QApplication::arguments().size() > 1) {
        open_view(QApplication::arguments()[1]);
    } else {
        // place some hints on a screen
        auto welcomeHint = new QLabel();
        welcomeHint->setTextFormat(Qt::RichText);
        QString t = "<font color=gray>";
        t += "<h1><u>Welcome</u></h1>";
        t += "<p />";
        t += "<ul>";
        t += "<li>Use menu <b>File</b> -> <b>Open</b> to load PLY file</li>";
        t += "<li>Use with command line: ./pointcloudviewer your_cloud.ply</li>";
        t += "<li><h2>Navigation hints</h2></li>";
        t += "<ul>";
        t += "<li>Use mouse to rotate camera</li>";
        t += "<li>Use mouse scroll to zoom</li>";
        t += "</ul>";
        t += "<ul>";
        t += "</ul>";
        t += "<ul>";
        t += "<font>";
        welcomeHint->setText(t);
        welcomeHint->setMargin(50);
        setCentralWidget(welcomeHint);
    }

    m_update_timer = std::make_unique<QTimer>();
    m_update_timer->setInterval(static_cast<int>(10));
    m_update_timer->start();
    connect(m_update_timer.get(), &QTimer::timeout, this, &MainWindow::update_stats);
}

MainWindow::~MainWindow()
{
    if (nullptr != m_rendering_dialog) {
        m_rendering_dialog->deleteLater();
    }
}

void MainWindow::create_menu_bar()
{
    QMenuBar *menuBar = new QMenuBar;
    setMenuBar(menuBar);
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *openFile = new QAction(tr("&Open"), fileMenu);
    fileMenu->addAction(openFile);
    connect(openFile, &QAction::triggered, this, &MainWindow::open_file_dialog);
    QAction *actionResetView = new QAction(tr("&Reset Camera View"), this);
    fileMenu->addAction(actionResetView);
    connect(actionResetView, &QAction::triggered, this, &MainWindow::reset_camera_view);
    QAction *closeView = new QAction(tr("&Close"), fileMenu);
    fileMenu->addAction(closeView);
    connect(closeView, &QAction::triggered, this, &MainWindow::close_view);

    QMenu *settingsMenu = menuBar->addMenu(tr("&Settings"));
    QAction *renderingSettings = new QAction(tr("&Camera Rendering"), fileMenu);
    settingsMenu->addAction(renderingSettings);
    connect(renderingSettings, &QAction::triggered, this, &MainWindow::create_rendering_dialog);

    QAction *point_cloud_control = new QAction(tr("&Point Cloud Control"), fileMenu);
    settingsMenu->addAction(point_cloud_control);
    connect(point_cloud_control, &QAction::triggered, this, &MainWindow::create_pc_control_dialog);

    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    QAction *aboutDialog = new QAction(tr("&About"), helpMenu);
    helpMenu->addAction(aboutDialog);
    connect(aboutDialog, &QAction::triggered, this, &MainWindow::create_about_dialog);
}

void MainWindow::open_file_dialog()
{
    if (m_gl_window)
        m_gl_window->stop_rendering();

    const QString filePath = QFileDialog::getOpenFileName(this, tr("Open PLY file"), "../resources/pointclouds/", tr("PLY Files (*.ply)"));
    qDebug() << "Open:" << filePath;

    if (!filePath.isEmpty()) {
        open_view(filePath);
    }

    if (m_gl_window){
        m_gl_window->start_rendering();
    }
}

void MainWindow::open_view(const QString& ply_path)
{
    // create new widget
    if (!m_gl_window) {
        m_gl_window = std::make_unique<ViewerWindow>();
        setCentralWidget(QWidget::createWindowContainer(m_gl_window.get(), this));
        const QRect desk = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(this));
        m_gl_window->resize(static_cast<int>(desk.width() * .8f), static_cast<int>(desk.height() * .8f));
    }
    m_gl_window->open_ply(ply_path.toStdString());
}


void MainWindow::close_view()
{
    if (!centralWidget())
        return;

    // destroy view
    centralWidget()->close();
    takeCentralWidget()->deleteLater();
    // remove source path from title
    setWindowTitle(m_title);
}

