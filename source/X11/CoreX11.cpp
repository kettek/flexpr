#include "CoreX11.hpp"
#include <X11/Xutil.h>
#include <ctype.h>
#include <iostream>
#include <string>

#include "Expressionator.hpp"

CoreX11::CoreX11() {
  m_display = XOpenDisplay(NULL);
  if (m_display == NULL) {
    std::cerr << "Cannot open Display" << std::endl;
    // TODO: throw
  }

  m_screen      = DefaultScreen(m_display);
  m_root_window = RootWindow(m_display, m_screen);

  m_tooltip.setup(m_display);
}
CoreX11::~CoreX11() {
  m_tooltip.cleanup();
  XCloseDisplay(m_display);
}

bool CoreX11::interceptLoop() {
  long        event_mask  = KeyPressMask | FocusChangeMask | PointerMotionMask | ExposureMask;
  KeySym      escape      = XKeysymToKeycode(m_display, XK_Escape);
  std::string calc_string;
  Window      focused_window;
  int         revert;
  XEvent      event;

  // Attach to focused window
  XGetInputFocus(m_display, &focused_window, &revert);
  XSelectInput(m_display, focused_window, event_mask);
  XGrabKeyboard(m_display, focused_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);

  m_tooltip.repositionSelf();
  m_tooltip.show();

  while (1) {
    XNextEvent(m_display, &event);

    if (event.type == FocusOut) {
      // Remove old focus
      if (focused_window != m_root_window) {
        XSelectInput(m_display, focused_window, 0);
      }
      XUngrabKeyboard(m_display, CurrentTime);
      // Focus on new
      XGetInputFocus(m_display, &focused_window, &revert);
      if (focused_window == PointerRoot) {
        focused_window = m_root_window;
      }
      XSelectInput(m_display, focused_window, event_mask);
      XGrabKeyboard(m_display, focused_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    } else if (event.type == KeyPress) {
      if (event.xkey.keycode == escape) {
        break;
      }
      char buf[17];
      int len;
      KeySym keysym;
      XComposeStatus comp;
      len = XLookupString(&event.xkey, buf, 16, &keysym, &comp);
      if (len > 0 && isprint(buf[0])) {
        buf[len] = '\0';
        calc_string += buf;
      } else {
        if ((int)keysym == 65288) { // delete
          if (calc_string.size() > 0) {
            calc_string.resize(calc_string.size() -1);
          }
        } else if ((int)keysym == 65293) { // return
          std::string out = Expressionator::evaluateInfix(calc_string);
          XKeyEvent key_event;
          for (size_t i = 0; i < out.length(); i++) {
            key_event = createKeyEvent(m_display, focused_window, m_root_window, true, out[i], 0);
            XSendEvent(m_display, focused_window, True, KeyPressMask, (XEvent*)&key_event);
            key_event = createKeyEvent(m_display, focused_window, m_root_window, false, out[i], 0);
            XSendEvent(m_display, focused_window, True, KeyReleaseMask, (XEvent*)&key_event);
          }
          break;
        }
      }
      m_tooltip.setText(calc_string);
    } else if (event.type == MotionNotify) {
      m_tooltip.repositionSelf();
    } else if (event.type == Expose) {
      m_tooltip.render();
    }
  }

  // Detach from focused window
  XUngrabKeyboard(m_display, CurrentTime);
  XSelectInput(m_display, focused_window, 0);

  m_tooltip.hide();
  return false;
}

bool CoreX11::hotkeyLoop() {
  int           hotkey_keycode    = XKeysymToKeycode(m_display, XK_C);
  unsigned int  hotkey_modifiers  = Mod1Mask | ShiftMask;
  
  XGrabKey(m_display, hotkey_keycode, hotkey_modifiers, m_root_window, True, GrabModeAsync, GrabModeAsync);
  XSelectInput(m_display, m_root_window, KeyReleaseMask);

  XEvent      event;
  while (1) {
    XNextEvent(m_display, &event);

    if (event.type == KeyPress && event.xkey.keycode == hotkey_keycode) {
      break;
    }
  }

  XSelectInput(m_display, m_root_window, 0);
  XUngrabKey(m_display, hotkey_keycode, hotkey_modifiers, m_root_window);
  return false;
}

XKeyEvent CoreX11::createKeyEvent(Display *display, Window &window, Window &root_window, bool press, int keycode, int modifiers) {
  XKeyEvent event;
  event.display     = display;
  event.window      = window;
  event.root        = root_window;
  event.subwindow   = None;
  event.time        = CurrentTime;
  event.x           = 1;
  event.y           = 1;
  event.x_root      = 1;
  event.y_root      = 1;
  event.same_screen = True;
  event.keycode     = XKeysymToKeycode(display, (KeySym)keycode);
  event.state       = modifiers;
  event.type        = press ? KeyPress : KeyRelease;

  return event;
}
