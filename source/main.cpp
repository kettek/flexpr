#if defined( USE_X11 )
#if defined ( USE_XINPUT2 )
#include "CoreXInput2.hpp"
#else
#include "CoreX11.hpp"
#endif
#elif defined ( USE_WIN32 )
#include "CoreWin32.hpp"
#endif

#include <iostream>

int main(int argc, char *argv[]) {
#if defined ( USE_X11 )
#if defined ( USE_XINPUT2 )
  CoreXInput2 core;
#else
  CoreX11 core;
#endif
#elif defined ( USE_WIN32 )
  CoreWin32 core;
#endif
  std::cout << "dang" << std::endl;

  while (1) {
    if (core.hotkeyLoop()) break;
    if (core.interceptLoop()) break;
  }
  return 0;
}
