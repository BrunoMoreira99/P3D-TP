project "BallPool"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Roose/src",
        "%{IncludeDir.glm}"
    }

    links { "Roose" }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
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

        postbuildcommands
        {
            "{COPYDIR} assets %{cfg.targetdir}/assets"
        }
