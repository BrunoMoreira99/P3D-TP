project "Roose"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "rspch.h"
    pchsource "src/rspch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "vendor/stb_image"
    }

    links
    {
        "vendor/GLEW/glew32s.lib",
        "vendor/GLFW/glfw3.lib",
        "opengl32.lib",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLEW_STATIC"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "RS_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RS_RELEASE" }
        runtime "Release"
        optimize "On"
