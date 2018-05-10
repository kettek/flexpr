#pragma once
#include "Tooltip.hpp"
#include <X11/Xlib.h>

class TooltipX11 : public Tooltip {
  public:
    TooltipX11();
    ~TooltipX11();
    int setup(Display* display);
    int cleanup();
    void move(int x, int y);
    void resize(int width, int height);
    void setText(const std::string text);
    void render();
    void repositionSelf();
    void resizeSelf();
    void show();
    void hide();
  private:
    Display*      m_display;
    Window        m_window;
    int           m_screen;
    GC            m_gc;
    XFontStruct*  m_font;
    unsigned long m_black_pixel;
    unsigned long m_white_pixel;
};
