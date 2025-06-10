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
        "src/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp"
    }

    includedirs
    {
        "src",
        "%{IncludeDir.GLEW}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links
    {
        "%{Library.GLEW}",
        "%{Library.GLFW}",
        "%{Library.OpenGL}"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLEW_STATIC",
        "GLFW_INCLUDE_NONE",
        "GLM_ENABLE_EXPERIMENTAL"
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

    filter "configurations:Dist"
        defines { "RS_DIST" }
        runtime "Release"
        optimize "On"
