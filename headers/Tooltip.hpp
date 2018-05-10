#pragma once
#include <string>

class Tooltip {
  protected:
    int m_x,      m_y;
    int m_width,  m_height;
    std::string   m_text;
  public:
    Tooltip();
    ~Tooltip();
    virtual void setText(const std::string text);
    virtual void move(int x, int y);
    virtual void resize(int width, int height);
    virtual void render() { };
    virtual void repositionSelf() { };
    virtual void resizeSelf() { };
    virtual void show() { };
    virtual void hide() { };
};
