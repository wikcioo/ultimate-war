workspace "UltimateWar"
    configurations { "Debug", "Release" }
    startproject "UltimateWar"

    filter "configurations:Debug"
        defines {
            "DEBUG",
            "SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE"
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "RELEASE",
            "SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF"
        }
        optimize "On"

project "UltimateWar"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	architecture "x86_64"
    warnings "Default"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.system}"
    objdir "obj/%{cfg.buildcfg}-%{cfg.system}"

    includedirs {
        "src/",
        "vendor/",
        "vendor/glm/",
        "vendor/imgui/",
        "vendor/glad/include/",
        "vendor/glfw/include/",
        "vendor/imgui/backends",
        "vendor/spdlog/include/",
        "vendor/freetype/include/"
    }

    files {
        "src/**.h",
        "src/**.cpp"
    }

    links { "GLFW", "GLM", "GLAD", "ImGui", "stb", "spdlog", "FreeType" }

    filter "system:linux"
        toolset "clang"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
        defines { "_WINDOWS" }

group "Dependencies"
    include "vendor/glfw.lua"
    include "vendor/glad.lua"
    include "vendor/glm.lua"
    include "vendor/imgui.lua"
    include "vendor/stb.lua"
    include "vendor/spdlog.lua"
    include "vendor/freetype.lua"
