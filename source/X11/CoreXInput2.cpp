#include "CoreXInput2.hpp"
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <ctype.h>
#include <iostream>
#include <string>

#include "Expressionator.hpp"

CoreXInput2::CoreXInput2() : CoreX11() {
}
/*CoreXInput2::~CoreXInput2() {
}*/

bool CoreXInput2::interceptLoop() {
  long        event_mask  = KeyPressMask | FocusChangeMask | PointerMotionMask | ExposureMask;
  KeySym      escape      = XKeysymToKeycode(m_display, XK_Escape);
  std::string calc_string;
  Window      focused_window;
  int         revert;
  XEvent      event;

  int xi_opcode;
  int xi_error;
  int xi_event;

  if (!XQueryExtension(m_display, "XInputExtension", &xi_opcode, &xi_event, &xi_error)) {
    std::cerr << "X Input extension unavailable." << std::endl;
    return true;
  }

  // Select for XI2
	XIEventMask ev_mask;
  unsigned char mask[1] = { 0 };
  ev_mask.deviceid = XIAllMasterDevices;
  ev_mask.mask_len = sizeof(mask);
  ev_mask.mask = mask;
  /* select for button and key events from all master devices */
  XISetMask(mask, XI_RawMotion);
  XISetMask(mask, XI_KeyPress);
  XISetMask(mask, XI_KeyRelease);
  //XISetMask(mask, XI_RawKeyPress);
  //XISetMask(mask, XI_RawKeyRelease);
  XISetMask(mask, XI_ButtonPress);
  XISetMask(mask, XI_FocusIn);
  XISetMask(mask, XI_FocusOut);
  XISetMask(mask, XI_Leave);

  XISelectEvents(m_display, m_root_window, &ev_mask, 1);
  // Attach to focused window
  XGetInputFocus(m_display, &focused_window, &revert);
  //XSelectInput(m_display, focused_window, event_mask);
  //XGrabKeyboard(m_display, focused_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  //XGrabKeyboard(m_display, m_root_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);


  XFlush(m_display);

  while (1) {
    XGenericEventCookie*  cookie = &event.xcookie;
    XIRawEvent*           re;

    XNextEvent(m_display, &event);
    //std::cout << "event" << std::endl;

    if (!XGetEventData(m_display, cookie)) continue;
    //if (cookie->type != GenericEvent || cookie->extension != xi_opcode || !XGetEventData(m_display, cookie)) continue;

    if (cookie->evtype == XI_RawMotion) {
      //std::cout << "The moose is loose" << std::endl;
    } else if (cookie->evtype == XI_KeyPress) {
      std::cout << "keypress" << std::endl;
    } else if (cookie->evtype == XI_ButtonPress) {
      std::cout << "buttonpress" << std::endl;
    } else if (cookie->evtype == XI_RawKeyPress) {
      XIDeviceEvent* xide = (XIDeviceEvent*) cookie->data;
      int keysym_count;
      KeySym *keysym = XGetKeyboardMapping(m_display, xide->detail, 1, &keysym_count);
      //std::cout << "key: " << xide->detail << std::endl;
      char *str = XKeysymToString(keysym[0]);
      std::cout << event.xkey.state << std::endl;
      if (str != NULL) {
        std::cout << str << std::endl;
//        calc_string += buf;
      }

      if (keysym != NULL) XFree(keysym);
    } else if (cookie->evtype == XI_KeyPress) {
      std::cout << "keypress" << std::endl;
    } else {
      std::cout << "?" << std::endl;
    }

    XFreeEventData(m_display, cookie);

  }
}

bool CoreXInput2::hotkeyLoop() {
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
