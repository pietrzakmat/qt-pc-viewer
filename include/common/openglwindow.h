#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <memory>
#include <iostream>

#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QMutexLocker>
#include <QEvent>
#include <QTimer>

#include <opengl_helper.hpp>

// custom update event
class UpdateEvent : public QEvent
{
public:
    UpdateEvent()
        : QEvent(static_cast<QEvent::Type>(ms_event_type))
    {}

    static int event_type() { return ms_event_type; }

private:
    static int ms_event_type;
};

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
public:
    /*!
     * \brief OpenGLWindow
     * Create the window object so the surface for QWindow is initialized.
     * If the OpenGL context need to be shared between windows,create externally shared context and set it with set_opengl_context(...)
     * \param parent
     */
    explicit OpenGLWindow(std::shared_ptr<QOpenGLContext> opengl_context = nullptr, QWindow *parent = nullptr);
//    virtual ~OpenGLWindow() {}

    static inline
    QSurfaceFormat create_format() {
        //******** OpenGL surface creation for QWindow START ********
        QSurfaceFormat format;
        format.setSamples(16);
        format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
        format.setSwapInterval(1); // attempt to switch ON the VSync. If =1 then VSync ON. Parameter defines number of frames of delay.

        return format;
    }

    void set_opengl_context(std::shared_ptr<QOpenGLContext> ctx) { m_context = ctx; m_gl_initialized = false; }
    std::shared_ptr<QOpenGLContext> opengl_context() { return m_context; }

//    QOpenGLPaintDevice *paint_device() { return m_open_gl_paint_device.get(); }
    std::size_t render_time() const { return m_render_time; }
    double fps() const { return m_fps; }

    void start_rendering();
    void stop_rendering();
    void render_now();

protected:
    virtual void initialize_gl() = 0;
    virtual void resizeGL( int width, int height ) = 0;
    virtual void paintGL() = 0;
    virtual void paint(QPainter &painter) = 0;

    bool event(QEvent *event) override;

private:
    std::shared_ptr<QOpenGLContext> m_context {nullptr};
    std::shared_ptr<QOpenGLPaintDevice> m_open_gl_paint_device {nullptr};
    bool m_gl_initialized {false};
    bool m_is_running {true};
    QMutex m_mutex;
    std::size_t m_render_time;
    double m_fps;
    std::chrono::steady_clock::time_point m_start_time {std::chrono::steady_clock::now()};
//    std::unique_ptr<QTimer> m_rendering_timer {nullptr};
};


#endif // OPENGLWINDOW_H
