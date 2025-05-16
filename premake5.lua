include "Dependencies.lua"

workspace "P3D-TP"
    architecture "x64"
    startproject "BallPool"

    configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
    include "Roose"
group ""

group "Applications"
    include "BallPool"
group ""
