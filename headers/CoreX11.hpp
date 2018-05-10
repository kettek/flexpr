#pragma once
#include "Core.hpp"
#include "TooltipX11.hpp"
#include <X11/Xlib.h>

class CoreX11 : public Core {
  public:
    CoreX11();
    ~CoreX11();
    bool hotkeyLoop();
    bool interceptLoop();
  protected:
    Display*    m_display;
    Window      m_root_window;
    int         m_screen;
    TooltipX11  m_tooltip;
    XKeyEvent   createKeyEvent(Display *display, Window &window, Window &root_window, bool press, int keycode, int modifiers);
};

