#include "CoreX11.hpp"
#include <X11/Xutil.h>
#include <ctype.h>
#include <iostream>
#include <string>

#include <unistd.h>
#include <cstring>
#include <limits.h>

#include "Expressionator.hpp"

CoreX11::CoreX11() {
  m_display = XOpenDisplay(NULL);
  if (m_display == NULL) {
    std::cerr << "Cannot open Display" << std::endl;
    // TODO: throw
  }

  // Set our keymaps
  m_keymap = {
    // Modifiers
    {"Ctrl", XK_Control_L}, {"LCtrl", XK_Control_L}, {"RCtrl", XK_Control_R},
    {"Alt", XK_Alt_L}, {"LAlt", XK_Alt_L}, {"RAlt", XK_Alt_R},
    {"Menu", XK_Menu}, {"LMenu", XK_Menu}, {"RMenu", XK_Menu},
    {"Shift", XK_Shift_L}, {"LShift", XK_Shift_L}, {"RShift", XK_Shift_R},
    // Odd
    {"Super", XK_Super_L}, {"SuperL", XK_Super_L}, {"SuperR", XK_Super_R},
    //{"Apps", XK_APPS},
    //{"Mail", XK_LAUNCH_MAIL}, {"App1", XK_LAUNCH_APP1}, {"App2", XK_LAUNCH_APP2},
    // Numbers
    {"0", XK_0}, {"1", XK_1}, {"2", XK_2}, {"3", XK_3}, {"4", XK_4}, {"5", XK_5}, {"6", XK_6}, {"7", XK_7}, {"8", XK_8}, {"9", XK_9}, 
    // Alphabet
    {"A", XK_A}, {"B", XK_B}, {"C", XK_C}, {"D", XK_D}, {"E", XK_E}, {"F", XK_F}, {"G", XK_G},
    {"H", XK_H}, {"I", XK_I}, {"J", XK_J}, {"K", XK_K}, {"L", XK_L}, {"M", XK_M}, {"N", XK_N},
    {"O", XK_O}, {"P", XK_P}, {"Q", XK_Q}, {"R", XK_R}, {"S", XK_S}, {"T", XK_T}, {"U", XK_U},
    {"V", XK_V}, {"W", XK_W}, {"X", XK_X}, {"Y", XK_Y}, {"Z", XK_Z},
    // Other
    {"Backspace", XK_BackSpace}, {"Tab", XK_Tab}, {"Clear", XK_Clear}, {"Return", XK_Return},
    {"Pause", XK_Pause}, {"CapsLock", XK_Caps_Lock}, {"Esc", XK_Escape}, {"Space", XK_KP_Space},
    {"PageUp", XK_Page_Up}, {"PageDown", XK_Page_Down}, {"End", XK_End}, {"Home", XK_Home},
    {"Left", XK_Left}, {"Right", XK_Right}, {"Down", XK_Down}, {"Up", XK_Up},
    {"Select", XK_Select}, {"Print", XK_Print}, {"Execute", XK_Execute},
    {"PrintScreen", XK_Print}, {"Insert", XK_Insert}, {"Delete", XK_Delete},
    {"Help", XK_Help}, {"NumLock", XK_Num_Lock}, {"ScrollLock", XK_Scroll_Lock},
    {"Num0", XK_KP_0}, {"Num1", XK_KP_1}, {"Num2", XK_KP_2},
    {"Num3", XK_KP_3}, {"Num4", XK_KP_4}, {"Num5", XK_KP_5},
    {"Num6", XK_KP_6}, {"Num7", XK_KP_7}, {"Num8", XK_KP_8},
    {"Num9", XK_KP_9},
    {"Plus", XK_KP_Add}, {"Minus", XK_KP_Subtract}, {"Multiply", XK_KP_Multiply},
    {"Divide", XK_KP_Divide},
    // Function Keys
    {"F1", XK_F1}, {"F2", XK_F2}, {"F3", XK_F3}, {"F4", XK_F4}, {"F5", XK_F5},
    {"F6", XK_F6}, {"F7", XK_F7}, {"F8", XK_F8}, {"F9", XK_F9}, {"F10", XK_F10},
    {"F11", XK_F11}, {"F12", XK_F12}, {"F13", XK_F13}, {"F14", XK_F14}, {"F15", XK_F15},
    {"F16", XK_F16}, {"F17", XK_F17}, {"F18", XK_F18}, {"F19", XK_F19}, {"F20", XK_F20},
    {"F21", XK_F21}, {"F22", XK_F22}, {"F23", XK_F23}, {"F24", XK_F24},
  };
  m_keymap_modifiers = {
    {"Alt", Mod1Mask}, {"Ctrl", ControlMask}, {"Shift", ShiftMask}, {"Super", Mod2Mask}
  };



  m_screen      = DefaultScreen(m_display);
  m_root_window = RootWindow(m_display, m_screen);

  m_tooltip.setup(m_display);
}
CoreX11::~CoreX11() {
  m_tooltip.cleanup();
  XCloseDisplay(m_display);
}

void CoreX11::getConfigurationPath() {
  char dest[PATH_MAX];
  memset(dest, 0, sizeof(dest));

  if (readlink("/proc/self/exe", dest, PATH_MAX) == -1) {
    perror("readlink");
    return;
  }
  m_config_path = dest;

  std::size_t found = m_config_path.find_last_of("/");
  m_config_path = m_config_path.substr(0, found);
  m_config_path += "/flexpr.cfg";
}
void CoreX11::loadConfiguration() {
  Core::loadConfiguration();
}


bool CoreX11::interceptLoop() {
  long        event_mask  = KeyPressMask | FocusChangeMask | PointerMotionMask;
  KeySym      escape      = XKeysymToKeycode(m_display, XK_Escape);
  std::string calc_string;
  Window      focused_window;
  int         revert;
  XEvent      event;

  // Attach to focused window
  XGetInputFocus(m_display, &focused_window, &revert);
  XSelectInput(m_display, focused_window, event_mask);
  // Listen for Exposure events from our tooltip
  XSelectInput(m_display, m_tooltip.m_window, ExposureMask);
  // Steal all keypresses
  XGrabKeyboard(m_display, focused_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);

  m_tooltip.show();
  m_tooltip.repositionSelf();

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

  // Stop selecting tooltip
  XSelectInput(m_display, m_tooltip.m_window, 0);

  m_tooltip.hide();
  return false;
}

bool CoreX11::hotkeyLoop() {
  // FIXME: We allow multiple keys to conf, but this code only supports one.
  int           hotkey_keycode    = XKeysymToKeycode(m_display, m_hotkey_vks);

  XGrabKey(m_display, hotkey_keycode, m_hotkey_modifiers, m_root_window, True, GrabModeAsync, GrabModeAsync);
  XSelectInput(m_display, m_root_window, KeyReleaseMask);

  XEvent      event;
  while (1) {
    XNextEvent(m_display, &event);

    if (event.type == KeyPress && event.xkey.keycode == hotkey_keycode) {
      break;
    }
  }

  XSelectInput(m_display, m_root_window, 0);
  XUngrabKey(m_display, hotkey_keycode, m_hotkey_modifiers, m_root_window);
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
