workspace "UltimateWar"
    configurations { "Debug", "Release" }
    startproject "UltimateWar"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"

project "UltimateWar"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
	architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}-%{cfg.system}"
    objdir "obj/%{cfg.buildcfg}-%{cfg.system}"

    includedirs {
        "include/",
        "vendor/glad/include/",
        "vendor/glfw/include/",
        "vendor/glm/",
        "vendor/imgui/",
        "vendor/imgui/backends",
        "vendor/spdlog/include/"
    }

    files {
        "src/*.cpp",
        "src/core/*.cpp"
    }

    links { "GLFW", "GLM", "GLAD", "ImGui" }

    filter "system:linux"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
        defines { "_WINDOWS" }

include "vendor/glfw.lua"
include "vendor/glad.lua"
include "vendor/glm.lua"
include "vendor/imgui.lua"
