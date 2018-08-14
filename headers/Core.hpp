#pragma once
#include <string>
#include <fstream>
#include <unordered_map>

class Core {
  public:
    virtual bool hotkeyLoop() { return false; };
    virtual bool interceptLoop() { return false; };
    virtual void loadConfiguration();
  protected:
    std::string  m_config_path;
    std::fstream m_config_file;
    std::unordered_map<std::string, std::string> m_config = {
      {"hotkey", "Alt+C"}
    };
    std::unordered_map<std::string, size_t> m_keymap;
    std::unordered_map<std::string, size_t> m_keymap_modifiers;
    unsigned int            m_hotkey_modifiers = 0;
    unsigned int            m_hotkey_vks = 0;
};
