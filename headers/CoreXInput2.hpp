#pragma once
#include "CoreX11.hpp"
#include "TooltipX11.hpp"
#include <X11/Xlib.h>

class CoreXInput2 : public CoreX11 {
  public:
    CoreXInput2();
    //~CoreXInput2();
    bool hotkeyLoop();
    bool interceptLoop();
};

