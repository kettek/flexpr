#include "TooltipWin32.hpp"
#include <iostream>
#include <WinStrings.hpp>

TooltipWin32::TooltipWin32() : Tooltip() {

}

TooltipWin32::~TooltipWin32() {
}

int TooltipWin32::setup() {
  resize(128, 24);

  m_window_handle = CreateWindowEx(
      WS_EX_TOPMOST|WS_EX_TOOLWINDOW, // Optional window styles
      L"EDIT",                        // Window class
      L"",                            // Window text
      WS_DLGFRAME|WS_POPUP,           // Window style
      m_x, m_y, m_width, m_height,    // Size and position
      NULL,                           // Parent window
      NULL,                           // Menu
      NULL,                           // Instance handle
      NULL                            // Additional application data
  );     

  if (m_window_handle == NULL) {
    MessageBox(NULL, L"Creating Window Failed!", L"Error!",
        MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }

  SetWindowLongPtr(m_window_handle, GWLP_USERDATA, (LONG)this);

  //SetFocus(GetDlgItem(m_window_handle, 0));

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
  UpdateWindow(m_window_handle);
}
void TooltipWin32::hide() {
  // Hide
  ShowWindow(m_window_handle, SW_HIDE);
  SetWindowText(m_window_handle, TEXT(""));
}

void TooltipWin32::setText(const std::string text) {
  Tooltip::setText(text);
  SetWindowText(m_window_handle, s2ws(text).c_str());
  resizeSelf();
}

void TooltipWin32::move(int x, int y) {
  Tooltip::move(x, y);
  MoveWindow(m_window_handle, m_x, m_y, m_width, m_height, FALSE);
}
void TooltipWin32::resize(int width, int height) {
  if (width < 64) width = 64;
  if (height < 24) height = 24;
  Tooltip::resize(width, height);
  MoveWindow(m_window_handle, m_x, m_y, m_width, m_height, TRUE); // render() called via WM_PAINT here
}

void TooltipWin32::render() {
  SetForegroundWindow(m_window_handle);
  HDC m_hdc = GetDC(m_window_handle);
  SIZE size;

  int len = GetWindowTextLength(m_window_handle);
  #ifdef UNICODE
  std::wstring buffer(len+1, '\0');
  GetWindowText(m_window_handle, (LPWSTR)buffer.c_str(), len+1);
  GetTextExtentPoint32(m_hdc, (LPWSTR)buffer.c_str(), len+1, &size);
  #else
  std::string buffer(len+1, '\0');
  GetWindowText(m_window_handle, (LPSTR)buffer.c_str(), len+1);
  GetTextExtentPoint32(m_hdc, s2ws(buffer).c_str(), len+1, &size);
  #endif
  resize(size.cx+16, size.cy);
}
