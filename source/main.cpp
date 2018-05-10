#ifdef __linux__
#include "CoreX11.hpp"
#endif

int main(int argc, char *argv[]) {
#ifdef __linux__
  CoreX11 core;
#endif

  while (1) {
    if (core.hotkeyLoop()) break;
    if (core.interceptLoop()) break;
  }
  return 0;
}
