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
    void repositionSelf();
    void resizeSelf();
    void show();
    void hide();
  private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    LRESULT CALLBACK windowMessageProcessor(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    WNDCLASS  m_window_class = { };
    HWND      m_window_handle;
};
