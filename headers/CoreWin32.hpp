#pragma once
#include "Core.hpp"
#include "TooltipWin32.hpp"
#include <unordered_map>

class CoreWin32 : public Core {
  public:
    CoreWin32();
    ~CoreWin32();
    bool hotkeyLoop();
    bool interceptLoop();
    bool showTrayIcon();
    bool hideTrayIcon();
    static LRESULT CALLBACK handleWindowProcedure(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
    bool handleTrayIconEvent(WPARAM w_param, LPARAM l_param);
    void getConfigurationPath();
    void loadConfiguration();
  protected:
    HWND            m_focused_window; // Window we wish to paste to
    HWND            m_window_handle; // Hidden window
    WNDCLASS        m_window_class = { };
    TooltipWin32    m_tooltip;
    HICON           m_icon;
    HMENU           m_menu;
    NOTIFYICONDATA  m_nid;
    UINT            m_hotkey_modifiers = 0;
    UINT            m_hotkey_vks = 0;
};

