project "spdlog"
    kind "None"
    architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.system}"
    objdir "obj/%{cfg.buildcfg}-%{cfg.system}"
