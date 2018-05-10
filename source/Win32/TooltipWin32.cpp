#include "TooltipWin32.hpp"
#include <iostream>

TooltipWin32::TooltipWin32() : Tooltip() {

}

TooltipWin32::~TooltipWin32() {
}

int TooltipWin32::setup() {
  const wchar_t CLASS_NAME[] = L"TooltipWin32 Class";

	m_window_class.lpfnWndProc    = WndProc;
  m_window_class.hInstance      = GetModuleHandle(0);
  m_window_class.lpszClassName  = CLASS_NAME;

  if (!RegisterClass(&m_window_class)) {
    MessageBox(NULL, L"Window Registration Failed!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }

  m_window_handle = CreateWindowEx(
      WS_EX_TOPMOST,                // Optional window styles
      CLASS_NAME,                   // Window class
      L"Text",                      // Window text
      WS_POPUP,                     // Window style
      m_x, m_y, m_width, m_height,  // Size and position
      NULL,                         // Parent window
      NULL,                         // Menu
      m_window_class.hInstance,     // Instance handle
      NULL                          // Additional application data
  );     

  if (m_window_handle == NULL) {
    MessageBox(NULL, L"Creating Window Failed!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }

  SetWindowLongPtr(m_window_handle, GWLP_USERDATA, (LONG)this);

  show();

  return 0;
}

int TooltipWin32::cleanup() {
  return 0;
}

void TooltipWin32::show() {
  // Do stuff

  // Prerender
  render();

  // Actually show here
  repositionSelf();

  ShowWindow(m_window_handle, SW_SHOW);
}
void TooltipWin32::hide() {
  // Hide
  ShowWindow(m_window_handle, SW_HIDE);
}

void TooltipWin32::setText(const std::string text) {
  Tooltip::setText(text);
  resizeSelf();
}

void TooltipWin32::move(int x, int y) {
  Tooltip::move(x, y);
  MoveWindow(m_window_handle, m_x, m_y, m_width, m_height, FALSE);
}
void TooltipWin32::resize(int width, int height) {
  Tooltip::resize(width, height);
  MoveWindow(m_window_handle, m_x, m_y, m_width, m_height, TRUE); // render() called via WM_PAINT here
}

void TooltipWin32::render() {
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(m_window_handle, &ps);

  FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

  EndPaint(m_window_handle, &ps);
}

void TooltipWin32::repositionSelf() {
  // call move(x,y)
}

void TooltipWin32::resizeSelf() {
  // call resize(w, h)
}

LRESULT CALLBACK TooltipWin32::WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  TooltipWin32* tooltip = (TooltipWin32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (tooltip) {
  	return tooltip->windowMessageProcessor(hwnd, msg, w_param, l_param);
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}

LRESULT CALLBACK TooltipWin32::windowMessageProcessor(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_PAINT:
      render();
      return 0;
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}
