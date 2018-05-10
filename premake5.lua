--
workspace "flexpr"
  configurations { "Debug", "Release" }

project "flexpr"
  kind "ConsoleApp"
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
    links { "X11", "Xi" }
    files { "source/X11/*.cpp" }
    configuration "without-xinput2"
      removelinks { "xi" }
      removefiles { "source/X11/CoreXInput2.cpp" }
