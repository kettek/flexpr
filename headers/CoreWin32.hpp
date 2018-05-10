#pragma once
#include "Core.hpp"
#include "TooltipWin32.hpp"

class CoreWin32 : public Core {
  public:
    CoreWin32();
    ~CoreWin32();
    bool hotkeyLoop();
    bool interceptLoop();
  protected:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    LRESULT CALLBACK windowMessageProcessor(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    HWND          m_window_handle; // Hidden window
    WNDCLASS      m_window_class = { };
    TooltipWin32  m_tooltip;
};

