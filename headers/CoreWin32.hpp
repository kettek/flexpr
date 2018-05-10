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
    HWND          m_window_handle; // Hidden window
    WNDCLASS      m_window_class = { };
    TooltipWin32  m_tooltip;
};

