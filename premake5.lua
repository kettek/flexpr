--
workspace "flexpr"
  configurations { "Debug", "Release" }

project "flexpr"
  kind "WindowedApp"
  language "C++"
  targetdir "%{cfg.buildcfg}"
  location ("build/" .. _ACTION)

  files { "source/*.cpp" }
  includedirs { "headers" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

  newoption {
    trigger = "without-xinput2",
    description = "Disable XInput2 support"
  }
  configuration { "linux" }
    defines { "USE_X11", "USE_XINPUT2" }
    files { "source/X11/*.cpp" }
    links { "X11", "Xi" }
    configuration "without-xinput2"
      removedefines { "USE_XINPUT2" }
      removefiles { "source/X11/CoreXInput2.cpp" }
      removelinks { "xi" }
  configuration { "win32" }
    defines { "USE_WIN32", "UNICODE" }
    files { "source/Win32/*.cpp" }
    entrypoint "mainCRTStartup"
