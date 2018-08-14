#include "Core.hpp"
#include <iostream>
#include <algorithm>

void Core::loadConfiguration() {	
  std::cout << m_config_path << std::endl;
  m_config_file.open(m_config_path);
  if (!m_config_file.is_open()) {
    std::cerr << "Couldn't open file " << m_config_path << ", using defaults" << std::endl;
    return;
  }
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