
#include <cassert>
#include <string>

#ifdef WIN32
#include "WinWindow.h"
#elif __APPLE__
#include "MacWindow.h"
#else
#include "LinWindow.h"
#endif

#include <glow/Log.h>
#include <glow/Context.h>
#include <glow/WindowEventHandler.h>
#include <glow/Window.h>

namespace glow
{


Window::Window()
:   m_eventHandler(nullptr)
,   m_context(nullptr)
,   m_quitOnDestroy(true)
{
#ifdef WIN32
    m_window = new WinWindow(*this);
#elif __APPLE__
    m_window = new MacWindow(*this);
#else
    m_window = new LinWindow(*this);
#endif
}

Window::~Window()
{
    delete m_window;
    m_window = nullptr;
}

WindowEventHandler * Window::eventHandler() const
{
    return m_eventHandler;
}

Context * Window::context() const
{
    return m_context;
}

int Window::handle() const
{
    return m_window->handle();
}

int Window::width() const
{
    return m_window->width();
}

int Window::height() const
{
    return m_window->height();
}

void Window::setQuitOnDestroy(const bool enable)
{
    m_quitOnDestroy = enable;
}

bool Window::create(
    const ContextFormat & format
,   const std::string & title
,   const unsigned int width
,   const unsigned int height)
{
    if (!m_window->create(format, title, width, height))
    {
        fatal() << "Creating native window failed.";
        return false;
    }

    assert(m_window->width()  == width);
    assert(m_window->height() == height);

    // setup context

    m_context = new Context();
    const bool result = m_context->create(handle(), format);

    promoteContext();

    return result;
}

bool Window::quitsOnDestroy() const
{
    return m_quitOnDestroy;
}

void Window::close()
{
    m_window->close();
}

void Window::promoteContext()
{
    if(!m_context || !m_eventHandler)
        return;

    m_context->makeCurrent();

    m_eventHandler->initializeEvent(*this);
    m_eventHandler->resizeEvent(*this, width(), height());

    m_context->doneCurrent();
}

void Window::show()
{
    m_window->show();
}

void Window::hide()
{
    m_window->hide();
}

void Window::fullScreen()
{
    m_window->fullScreen();
}

void Window::windowed()
{
    m_window->windowed();
}

void Window::attach(WindowEventHandler * eventHandler)
{
    m_eventHandler = eventHandler;

    if (!m_eventHandler)
        return;

    if (!m_context)
        return;

    promoteContext();
}

int Window::run()
{
#ifdef WIN32
    return WinWindow::run();
#elif __APPLE__
    return MacWindow::run();
#else
    return LinWindow::run();
#endif
}

void Window::quit(const int code)
{
#ifdef WIN32
    return WinWindow::quit(code);
#elif __APPLE__
    return MacWindow::quit(code);
#else
    return LinWindow::quit(code);
#endif
}

void Window::repaint()
{
    m_window->repaint();
}




void Window::onRepaint()
{
    m_context->makeCurrent();

    if (m_eventHandler)
        m_eventHandler->paintEvent(*this);

    m_context->swap();
    m_context->doneCurrent();
}

void Window::onResize()
{
    if (!m_context || !m_eventHandler)
        return;

    m_context->makeCurrent();
    m_eventHandler->resizeEvent(*this, width(), height());
    m_context->doneCurrent();
}

void Window::onIdle()
{
    if (m_eventHandler)
        m_eventHandler->idleEvent(*this);
}

void Window::onClose()
{
    if (m_context)
    {
        if (m_eventHandler)
        {
            m_context->makeCurrent();
            m_eventHandler->deinitializeEvent(*this);
            m_context->doneCurrent();
        }

        m_context->release();

        delete m_context;
        m_context = nullptr;
    }

    m_window->destroy();

    if (m_quitOnDestroy)
        quit(0);
}

//bool Window::onKeyPress(const unsigned short key)
//{
//    KeyEvent kpe(KeyEvent::KeyPress, key);
//    
//
//    m_keysPressed.insert(kpe.key());
//    return kpe.isAccepted();
//}
//
//
//bool Window::onKeyRelease(const unsigned short key)
//{
//    KeyEvent kre(KeyEvent::KeyRelease, key);
//
//    switch (kre.key())
//    {
//    case KeyEvent::KeyEscape:
//        kre.accept();
//        SendMessage(m_hWnd, WM_CLOSE, NULL, NULL);
//        break;
//
//    case KeyEvent::KeyReturn:
//        if (m_keysPressed.find(KeyEvent::KeyAlt) != m_keysPressed.cend())
//        {
//            kre.accept();
//            toggleMode();
//        }
//        break;
//
//    default: 
//        break;
//    }
//
//    const auto f = m_keysPressed.find(kre.key());
//    if (f != m_keysPressed.cend())
//        m_keysPressed.erase(f);
//
//    return kre.isAccepted();
//}

} // namespace glow
