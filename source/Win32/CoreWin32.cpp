#include "CoreWin32.hpp"
#include <windows.h>
#include <iostream>

#include "Expressionator.hpp"

CoreWin32::CoreWin32() {
  //
  const wchar_t CLASS_NAME[] = L"CoreWin32 Class";

	m_window_class.lpfnWndProc    = DefWindowProc;
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

  // Create tooltip, etc.
  m_tooltip.setup();
}
CoreWin32::~CoreWin32() {
  // Delete tooltip, etc.
  m_tooltip.cleanup();
}

bool CoreWin32::interceptLoop() {
  std::cout << "In interceptLoop" << std::endl;

  RAWINPUTDEVICE r_id[1];
  r_id[0].usUsagePage = 0x01;
  r_id[0].usUsage = 0x02;
  r_id[0].dwFlags = RIDEV_INPUTSINK;
  r_id[0].hwndTarget = m_window_handle;
  if (RegisterRawInputDevices(r_id, 1, sizeof(r_id[0])) == FALSE) {
    MessageBox(NULL, L"Could not register for mouse events!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
  }

  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0) {
    if (msg.message == WM_QUIT) {
    } else if (msg.message == WM_INPUT) {
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
      std::cout << bytes_copied << std::endl;
      RAWINPUT* raw = (RAWINPUT*) lpb;
      std::cout << raw->header.dwType << std::endl;
      if (raw->header.dwType == RIM_TYPEMOUSE) {
        POINT p;
        if (GetCursorPos(&p)) {
          m_tooltip.move(p.x, p.y);
        }
      }

      delete[] lpb;
      //DefWindowProc(m_window_handle, msg.message, msg.wParam, msg.lParam);
    }
    if (msg.message == 275) continue;
    std::cout << "oh: " << msg.message << std::endl;
  }

  // Capture all key presses
  m_tooltip.hide();
  return false;
}

bool CoreWin32::hotkeyLoop() {
  std::cout << "In hotkeyLoop" << std::endl;
  if (!RegisterHotKey(NULL, 1, MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 0x42)) {
    std::cerr << "Failed to register hotkey" << std::endl;
    return true;
  }

  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0) {
    if (msg.message == WM_HOTKEY) {
      std::cout << "Received hotkey" << std::endl;
      m_tooltip.show();
      break;
    }
  }

  UnregisterHotKey(NULL, 1);

  return false;
}