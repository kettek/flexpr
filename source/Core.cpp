#include "Core.hpp"
#include <iostream>
#include <algorithm>

void Core::loadConfiguration() {	
  std::cout << m_config_path << std::endl;
  m_config_file.open(m_config_path);
  if (!m_config_file.is_open()) {
    std::cerr << "Couldn't open file " << m_config_path << ", using defaults" << std::endl;
  } else {
    std::string line;
    while (std::getline(m_config_file, line)) {
      if (line.size() <= 0) continue;
      std::string key = "";
      std::string value = "";
  	
      auto split_pos = line.find("=");
      if (split_pos == std::string::npos) {
        key = line;
      } else {
        key = line.substr(0, split_pos);
        value = line.substr(split_pos+1, line.length());
      }
      // Remove white-space
      key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
      value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
  
      m_config[key] = value;
    }
  }
  
  // Get optimized hotkey
  size_t pos = 0;
  std::string hotkey_str = m_config["hotkey"];
  std::string token;
  while ((pos = hotkey_str.find("+")) != std::string::npos) {
    token = hotkey_str.substr(0, pos);
    auto it = m_keymap_modifiers.find(token);
    if (it != m_keymap_modifiers.end()) {
      m_hotkey_modifiers = m_hotkey_modifiers | it->second;
    } else {
      it = m_keymap.find(token);
      if (it != m_keymap.end()) {
        m_hotkey_vks = m_hotkey_vks | it->second;
      }
    }
    hotkey_str.erase(0, pos + 1);
  }
  auto it = m_keymap_modifiers.find(hotkey_str);
  if (it != m_keymap_modifiers.end()) {
    m_hotkey_modifiers = m_hotkey_modifiers | it->second;
  } else {
    it = m_keymap.find(hotkey_str);
    if (it != m_keymap.end()) {
      m_hotkey_vks = m_hotkey_vks | it->second;
    }
  }
  std::cout << "hotkeys: " << m_config["hotkey"] << " - " << m_hotkey_vks << "x" << m_hotkey_modifiers << std::endl;

}
