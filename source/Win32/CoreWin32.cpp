#include "CoreWin32.hpp"
#include <windows.h>
#include <Shellapi.h>
#include <Commctrl.h> // Icon(s)
#include <iostream>
#include <algorithm>

#include "Expressionator.hpp"

#include "WinStrings.hpp"

#define ID_TRAY_APP_ICON  5000
#define ID_TRAY_EXIT      3000
#define WM_TRAYICON       ( WM_USER + 1 )

CoreWin32::CoreWin32() {
  //
  const wchar_t CLASS_NAME[] = L"CoreWin32 Class";

  m_window_class.lpfnWndProc    = CoreWin32::handleWindowProcedure;
  m_window_class.hInstance      = GetModuleHandle(0);
  m_window_class.lpszClassName  = CLASS_NAME;

  if (!RegisterClass(&m_window_class)) {
    MessageBox(NULL, L"Window Registration Failed!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
  }

  m_window_handle = CreateWindowEx(
      0,                            // Optional window styles
      CLASS_NAME,                   // Window class
      L"",                          // Window text
      WS_POPUP,                     // Window style
      0, 0, 0, 0,                   // Size and position
      NULL,                         // Parent window
      NULL,                         // Menu
      m_window_class.hInstance,     // Instance handle
      NULL                          // Additional application data
  );

  if (m_window_handle == NULL) {
    MessageBox(NULL, L"Creating Window Failed!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
  }

  SetWindowLongPtr(m_window_handle, GWLP_USERDATA, (LONG)this);

  // Load icon
  m_icon = static_cast<HICON>(LoadImage(NULL,
            MAKEINTRESOURCE(IDI_APPLICATION),
            IMAGE_ICON,
            0, 0,
            LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE));

  showTrayIcon();

  // Add keymap
  m_keymap = {
    // Modifiers
    {"Ctrl", VK_CONTROL}, {"LCtrl", VK_LCONTROL}, {"RCtrl", VK_RCONTROL},
    {"Alt", VK_MENU}, {"LAlt", VK_LMENU}, {"RAlt", VK_RMENU},
    {"Menu", VK_MENU}, {"LMenu", VK_LMENU}, {"RMenu", VK_RMENU},
    {"Shift", VK_SHIFT}, {"LShift", VK_LSHIFT}, {"RShift", VK_RSHIFT},
    // Odd
    {"Super", VK_LWIN}, {"SuperL", VK_LWIN}, {"SuperR", VK_RWIN},
    {"Apps", VK_APPS},
    {"Mail", VK_LAUNCH_MAIL}, {"App1", VK_LAUNCH_APP1}, {"App2", VK_LAUNCH_APP2},
    // Numbers
    {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34}, {"5", 0x35}, {"6", 0x36},
    {"8", 0x37}, {"9", 0x38}, 
    // Alphabet
    {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45}, {"F", 0x46}, {"G", 0x47},
    {"H", 0x48}, {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E},
    {"O", 0x4F}, {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54}, {"U", 0x55},
    {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},
    // Other
    {"Backspace", VK_BACK}, {"Tab", VK_TAB}, {"Clear", VK_CLEAR}, {"Return", VK_RETURN},
    {"Pause", VK_PAUSE}, {"CapsLock", VK_CAPITAL}, {"Esc", VK_ESCAPE}, {"Space", VK_SPACE},
    {"PageUp", VK_PRIOR}, {"PageDown", VK_NEXT}, {"End", VK_END}, {"Home", VK_HOME},
    {"Left", VK_LEFT}, {"Right", VK_RIGHT}, {"Down", VK_DOWN}, {"Up", VK_UP},
    {"Select", VK_SELECT}, {"Print", VK_PRINT}, {"Execute", VK_EXECUTE},
    {"PrintScreen", VK_SNAPSHOT}, {"Insert", VK_INSERT}, {"Delete", VK_DELETE},
    {"Help", VK_HELP}, {"NumLock", VK_NUMLOCK}, {"ScrollLock", VK_SCROLL},
    {"Num0", VK_NUMPAD0}, {"Num1", VK_NUMPAD1}, {"Num2", VK_NUMPAD2},
    {"Num3", VK_NUMPAD3}, {"Num4", VK_NUMPAD4}, {"Num5", VK_NUMPAD5},
    {"Num6", VK_NUMPAD6}, {"Num7", VK_NUMPAD7}, {"Num8", VK_NUMPAD8},
    {"Num9", VK_NUMPAD9},
    {"Plus", VK_ADD}, {"Minus", VK_SUBTRACT}, {"Multiply", VK_MULTIPLY},
    {"Divide", VK_DIVIDE},
    // Function Keys
    {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
    {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10},
    {"F11", VK_F11}, {"F12", VK_F12}, {"F13", VK_F13}, {"F14", VK_F14}, {"F15", VK_F15},
    {"F16", VK_F16}, {"F17", VK_F17}, {"F18", VK_F18}, {"F19", VK_F19}, {"F20", VK_F20},
    {"F21", VK_F21}, {"F22", VK_F22}, {"F23", VK_F23}, {"F24", VK_F24}
  };
  
  m_keymap_modifiers = {
    {"Alt", MOD_ALT}, {"Ctrl", MOD_CONTROL}, {"Shift", MOD_SHIFT}, {"Meta", MOD_WIN}
  };

  // Create tooltip, etc.
  m_tooltip.setup();
}
CoreWin32::~CoreWin32() {
  hideTrayIcon();

  // Delete tooltip, etc.
  m_tooltip.cleanup();
}

void CoreWin32::getConfigurationPath() {
  wchar_t buffer[MAX_PATH];
  GetModuleFileName(NULL, buffer, MAX_PATH);
  // conver wchar_t to char*
  size_t wide_size = wcslen(buffer) + 1;
  size_t converted_chars = 0;
  const size_t new_size = wide_size*2;
  char* c_string = new char[new_size];
  wcstombs_s(&converted_chars, c_string, new_size, buffer, _TRUNCATE);
  
  m_config_path = c_string;
  // Remove exe name
  std::size_t found = m_config_path.find_last_of("\\");
  m_config_path = m_config_path.substr(0, found);
  m_config_path += "\\flexpr.cfg";
  
  delete[] c_string;
}

void CoreWin32::loadConfiguration() {
  Core::loadConfiguration();
}

bool CoreWin32::interceptLoop() {

  POINT p;
  if (GetCursorPos(&p)) {
    m_focused_window = WindowFromPoint(p);
    m_tooltip.move(p.x+16, p.y+16);
  }

  m_tooltip.show();

  RAWINPUTDEVICE r_id[1];
  r_id[0].usUsagePage = 0x01;
  r_id[0].usUsage = 0x02;
  r_id[0].dwFlags = RIDEV_INPUTSINK;
  r_id[0].hwndTarget = m_window_handle;
  if (RegisterRawInputDevices(r_id, 1, sizeof(r_id[0])) == FALSE) {
    MessageBox(NULL, L"Could not register for mouse events!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
  }
  // I suppose we'll re-register the hotkey here to allow canceling -- shouldn't unregister it ever, I think.
  if (!RegisterHotKey(NULL, 1, MOD_NOREPEAT | m_hotkey_modifiers, m_hotkey_vks)) {
    std::cerr << "Failed to register hotkey" << std::endl;
    return true;
  }

  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0) {
    if (msg.message == WM_INPUT) {
      UINT dw_size;
      LPBYTE lpb;

      GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dw_size, sizeof(RAWINPUTHEADER));
      lpb = new BYTE[dw_size];
      if (lpb == NULL) {
        continue;
      }

      UINT bytes_copied = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dw_size, sizeof(RAWINPUTHEADER));
      if (bytes_copied != dw_size) {
        //std::cout << "GetRawInputData does not return correct size!" << std::endl;
      }
      RAWINPUT* raw = (RAWINPUT*) lpb;
      if (raw->header.dwType == RIM_TYPEMOUSE) {
        POINT p;
        if (GetCursorPos(&p)) {
          m_focused_window = WindowFromPoint(p);
          m_tooltip.move(p.x+16, p.y+16);
        }
      }

      delete[] lpb;
      m_tooltip.render();
    } else if (msg.message == WM_KEYDOWN) {
      if (msg.wParam == VK_RETURN) {
        // FIXME: this is some hot garbage.
        int len = GetWindowTextLength(m_tooltip.m_window_handle);
        #ifdef UNICODE
        std::wstring buffer(len+1, '\0');
        GetWindowText(m_tooltip.m_window_handle, (LPWSTR)buffer.c_str(), len+1);
        std::string out = Expressionator::evaluateInfix(ws2s(buffer));
        #else
        std::string buffer(len+1, '\0');
        GetWindowText(m_tooltip.m_window_handle, (LPSTR)buffer.c_str(), len+1);
        std::string out = Expressionator::evaluateInfix(buffer);
        #endif

        INPUT *keystroke;
        UINT character_count, keystrokes_to_send, keystrokes_sent;

        if (m_focused_window != NULL) {
          SetForegroundWindow(m_focused_window);
          // Fill in the array of keystrokes to send.
          std::wstring text = s2ws(out);
          character_count = text.size();
          keystrokes_to_send = character_count * 2;
          keystroke = new INPUT[ keystrokes_to_send ];
          for(UINT i = 0; i < character_count; ++i ) {
              keystroke[ i * 2 ].type = INPUT_KEYBOARD;
              keystroke[ i * 2 ].ki.wVk = 0;
              keystroke[ i * 2 ].ki.wScan = text[i];
              keystroke[ i * 2 ].ki.dwFlags = KEYEVENTF_UNICODE;
              keystroke[ i * 2 ].ki.time = 0;
              keystroke[ i * 2 ].ki.dwExtraInfo = GetMessageExtraInfo();
    
              keystroke[ i * 2 + 1 ].type = INPUT_KEYBOARD;
              keystroke[ i * 2 + 1 ].ki.wVk = 0;
              keystroke[ i * 2 + 1 ].ki.wScan = text[i];
              keystroke[ i * 2 + 1 ].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
              keystroke[ i * 2 + 1 ].ki.time = 0;
              keystroke[ i * 2 + 1 ].ki.dwExtraInfo = GetMessageExtraInfo();
          }
    
          //Send the keystrokes.
          keystrokes_sent = SendInput( ( UINT )keystrokes_to_send, keystroke, sizeof( *keystroke ) );
          delete [] keystroke;
          m_tooltip.hide();
          UnregisterHotKey(NULL, 1);
          return false;
        }
      } else if (msg.wParam == VK_ESCAPE) {
        UnregisterHotKey(NULL, 1);
        m_tooltip.hide();
        return false;
      }
      m_tooltip.render();
    } else if (msg.message == WM_HOTKEY) {
      UnregisterHotKey(NULL, 1);
      m_tooltip.hide();
      return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return true;
}

bool CoreWin32::hotkeyLoop() {
  if (!RegisterHotKey(NULL, 1, MOD_NOREPEAT | m_hotkey_modifiers, m_hotkey_vks)) {
    std::cerr << "Failed to register hotkey" << std::endl;
    return true;
  }

  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0) {
    if (msg.message == WM_HOTKEY) {
      UnregisterHotKey(NULL, 1);
      return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return true;
}

LRESULT CALLBACK CoreWin32::handleWindowProcedure(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
  CoreWin32* core = (CoreWin32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (u_msg == WM_TRAYICON) {
    core->handleTrayIconEvent(w_param, l_param);
  }
  return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

bool CoreWin32::showTrayIcon() {
  m_nid                   = {sizeof(m_nid)};
  m_nid.cbSize            = sizeof(NOTIFYICONDATA);
  m_nid.hWnd              = m_window_handle;
  m_nid.uID               = ID_TRAY_APP_ICON;
  m_nid.uFlags            = NIF_ICON | NIF_MESSAGE;
  m_nid.hIcon             = m_icon;
  m_nid.uCallbackMessage  = WM_TRAYICON;

  // Create tray menu icon
  m_menu = CreatePopupMenu();
  AppendMenu(m_menu, MF_STRING, ID_TRAY_EXIT, TEXT("Quit flexpr"));

  Shell_NotifyIcon(NIM_ADD, &m_nid);
  m_nid.uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIcon(NIM_SETVERSION, &m_nid);

  return true;
}

bool CoreWin32::hideTrayIcon() {
  DestroyMenu(m_menu);
  Shell_NotifyIcon(NIM_DELETE, &m_nid);
  return true;
}

bool CoreWin32::handleTrayIconEvent(WPARAM w_param, LPARAM l_param) {
  POINT point;
  UINT click_id;

  if (LOWORD(l_param) == WM_CONTEXTMENU) {
    GetCursorPos(&point);
    SetForegroundWindow(m_window_handle); // Allows the popup to close on focus loss
    click_id = TrackPopupMenu(
      m_menu,
      TPM_RETURNCMD | TPM_NONOTIFY,
      point.x,
      point.y,
      0,
      m_window_handle,
      NULL
    );
  
    if (click_id == ID_TRAY_EXIT) {
      PostQuitMessage(0);
    }
  }

  return true;
}
