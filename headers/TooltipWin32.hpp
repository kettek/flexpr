#pragma once
#include "Tooltip.hpp"
#include <windows.h>

class TooltipWin32 : public Tooltip {
  public:
    TooltipWin32();
    ~TooltipWin32();
    int setup();
    int cleanup();
    void move(int x, int y);
    void resize(int width, int height);
    void setText(const std::string text);
    void render();
    void show();
    void hide();
  private:
    friend class CoreWin32;
    HWND      m_window_handle;
};
