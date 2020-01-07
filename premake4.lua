#!lua

includeDirList = { 
    "shared",
    "shared/gl3w",
    "shared/imgui",
    "shared/include"
}

buildOptions = {"-std=c++11"}

-- Get the current OS platform
PLATFORM = os.get()

-- Build lua-5.3.1 library and copyt it into <cs488_root>/lib if it is not
-- already present.
if not os.isfile("lib/liblua.a") then
    os.chdir("shared/lua-5.3.1")

    if PLATFORM == "macosx" then
        os.execute("make macosx")
    elseif PLATFORM == "linux" then
        os.execute("make linux")
    elseif PLATFORM == "windows" then
        os.execute("make mingw")
    end

    os.chdir("../../")
    os.mkdir("lib")
    os.execute("cp shared/lua-5.3.1/src/liblua.a lib/")
end


solution "BuildStaticLibs"
    configurations { "Debug", "Release" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    -- Build lodepng static library
    project "lodepng"
        kind "StaticLib"
        language "C++"
        location "build"
        objdir "build"
        targetdir "lib"
        includedirs (includeDirList)
        includedirs {
            "shared/lodepng"
        }
        files { 
            "shared/lodepng/lodepng.cpp"
        }
