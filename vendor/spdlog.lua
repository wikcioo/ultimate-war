project "spdlog"
    architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.system}"
    objdir "obj/%{cfg.buildcfg}-%{cfg.system}"

    filter "system:linux"
        kind "StaticLib"

    filter "system:windows"
        kind "None"
