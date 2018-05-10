#include "TooltipX11.hpp"
#include <X11/Xutil.h>
#include <X11/Xatom.h>

TooltipX11::TooltipX11() : Tooltip() {
}

TooltipX11::~TooltipX11() {
}

int TooltipX11::setup(Display* display) {
  m_display = display;

  // Get defaults
  m_screen = DefaultScreen(m_display);

  // Create window
  m_window = XCreateWindow(m_display, RootWindow(m_display, m_screen),
      m_x, m_y, m_width, m_height,
      0, DefaultDepth(m_display, m_screen), InputOutput,
      CopyFromParent,
      0, NULL);

  // Setup GC
  m_gc          = XCreateGC(m_display, m_window, 0, 0);
  m_white_pixel = WhitePixel(m_display, m_screen);
  m_black_pixel = BlackPixel(m_display, m_screen);
  XSetBackground(m_display, m_gc, m_white_pixel);
  XSetForeground(m_display, m_gc, m_black_pixel);

  // Load default font
  const char* fontname = "fixed";
  m_font = XLoadQueryFont(m_display, fontname);
  if (!m_font) {
    //std::cerr << "Cannot open Font" << std::endl;
    return 1;
  }
  XSetFont(m_display, m_gc, m_font->fid);

  return 0;
}

int TooltipX11::cleanup() {
  XFreeGC(m_display, m_gc);
  XDestroyWindow(m_display, m_window);
}

void TooltipX11::show() {
  // Setup WM Hinting
  Atom wm_type          = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False);
  Atom wm_type_value    = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_TOOLTIP", False);
  XChangeProperty(m_display, m_window, wm_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_type_value, 1);

  Atom wm_state         = XInternAtom(m_display, "_NET_WM_STATE", False);
  Atom wm_state_value   = XInternAtom(m_display, "_NET_WM_STATE_ABOVE", False);
  XChangeProperty(m_display, m_window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_state_value, 1);

  render();

  XMapWindow(m_display, m_window);
  repositionSelf();
}
void TooltipX11::hide() {
  XUnmapWindow(m_display, m_window);
}

void TooltipX11::setText(const std::string text) {
  Tooltip::setText(text);
  resizeSelf();
}

void TooltipX11::move(int x, int y) {
  Tooltip::move(x, y);
  XMoveWindow(m_display, m_window, m_x, m_y+m_height);
}
void TooltipX11::resize(int width, int height) {
  Tooltip::resize(width, height);
  XResizeWindow(m_display, m_window, m_width, m_height);
  render();
}

void TooltipX11::render() {
  XClearWindow(m_display, m_window);
  XSetForeground(m_display, m_gc, m_white_pixel);
  XFillRectangle(m_display, m_window, m_gc, 0, 0, m_width, m_height);

  XSetForeground(m_display, m_gc, m_black_pixel);
  XDrawString(m_display, m_window, m_gc, 0, m_height, m_text.c_str(), m_text.length());
}

void TooltipX11::repositionSelf() {
  Window    root_return;
  Window    child_return;
  int       root_x_return;
  int       root_y_return;
  int       child_x_return;
  int       child_y_return;
  unsigned  int mask_return;

  XQueryPointer(m_display, m_window, &root_return, &child_return, &root_x_return, &root_y_return, &child_x_return, &child_y_return, &mask_return);

  move(root_x_return, root_y_return);
}

void TooltipX11::resizeSelf() {
  int direction;
  int ascent;
  int descent;
  XCharStruct overall;

  XTextExtents(m_font, m_text.c_str(), m_text.length(),
      &direction, &ascent, &descent, &overall);

  resize(overall.width, ascent+2);
}
