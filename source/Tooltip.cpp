#include "Tooltip.hpp"

Tooltip::Tooltip() {
  m_x = 0,      m_y = 0;
  m_width = 8,  m_height = 8;
}

Tooltip::~Tooltip() {
}

void Tooltip::setText(const std::string text) {
  m_text = text;
  render();
}

void Tooltip::move(int x, int y) {
  m_x = x;
  m_y = y;
}
void Tooltip::resize(int width, int height) {
  m_width   = width;
  m_height  = height;
}
