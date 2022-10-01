#include "openglwindow.h"
#include <QCoreApplication>

#include <iostream>

int UpdateEvent::ms_event_type = QEvent::registerEventType();

OpenGLWindow::OpenGLWindow(std::shared_ptr<QOpenGLContext> opengl_context, QWindow *parent)
    : QWindow(parent)
    , m_context(opengl_context)
{
    setSurfaceType(OpenGLSurface);
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    format.setSwapInterval(0); // attempt to switch ON the VSync. If =1 then VSync ON. Parameter defines number of frames of delay.
    // Create the native window
    setFormat(format);
    create();

    // Schedule the first update - will happen on the main thread
    QCoreApplication::postEvent(this, new UpdateEvent);
}

bool OpenGLWindow::event(QEvent *event)
{
    if (event->type() == UpdateEvent::event_type()) {
        render_now();
        // Post an event to trigger another update. This relies upon
        // vsync to limit the rate of updates.
        if (m_is_running) {
            QCoreApplication::postEvent(this, new UpdateEvent);
        }
    } else {
        switch (event->type())
        {
//        case QEvent::UpdateRequest:
//            m_update_pending = false;
//            render_now();
//            break;
        case QEvent::Resize:
            if (m_gl_initialized) {
                m_context->makeCurrent(this);
                resizeGL(width(),height());
                m_open_gl_paint_device->setSize(this->size());
            }
            break;
        default:
            return QWindow::event(event);
        }
    }


    return QWindow::event(event);
}

void OpenGLWindow::start_rendering()
{
    QMutexLocker locker(&m_mutex); // blocking
    m_is_running = true;
    QCoreApplication::postEvent(this, new UpdateEvent);
}

void OpenGLWindow::stop_rendering()
{
    QMutexLocker locker(&m_mutex); // blocking
    m_is_running = false;
}

void OpenGLWindow::render_now()
{
    QMutexLocker locker(&m_mutex); // blocking
    if (!isExposed())
        return;

    m_start_time = std::chrono::steady_clock::now();

    if (!m_context) {
        // OpenGL context creation START
        m_context = std::make_shared<QOpenGLContext>();
        m_context->setFormat(requestedFormat());
        if (!m_context->create()) {
            std::cerr << "OpenGL context creation failed" << std::endl;
        }
        // OpenGL context creation END
    }

    m_context->makeCurrent(this);

    if (!m_gl_initialized) {
        m_gl_initialized = true;
        initializeOpenGLFunctions();
        m_open_gl_paint_device = std::make_shared<QOpenGLPaintDevice>(size());
        initialize_gl();
        resizeGL(width(), height());
    }

    paintGL();

    QPainter painter;
    painter.begin(m_open_gl_paint_device.get());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    paint(painter);
    painter.end();

    m_context->swapBuffers(this);
    m_context->doneCurrent();
    m_render_time = static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start_time).count()) > 1 ? static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start_time).count()) : m_render_time;
    m_fps = 1000. / static_cast<double>(m_render_time);
}
