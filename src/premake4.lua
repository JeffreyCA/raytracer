#!lua

includeDirList = {
    "../shared",
    "../shared/include"
}

libDirectories = { 
    "../lib"
}

if os.get() == "macosx" then
    linkLibs = {
        "lua",
        "lodepng"
    }
end

if os.get() == "linux" then
    linkLibs = {
        "lua",
        "lodepng",
        "stdc++",
        "dl",
        "pthread"
    }
end

if not os.isfile("out") then
    os.mkdir("out")
end

buildOptions = {"-std=c++11 -O2 -Wall -Wextra -Weffc++ -DPARALLEL"}

solution "raytrace"
    configurations { "Debug", "Release" }

    project "raytrace"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        includedirs (includeDirList)
        files { "*.cpp" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
