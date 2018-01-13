workspace "widget"

language "C++"
cppdialect "C++17"

configurations {
	"debug",
	"release"
}

includedirs {
	"external/stx/include",
	"external/stxmath/include",
	"external/freetype2/include"
}

optimize "Debug"
symbols "On"

project "widget"
	kind "StaticLib"
	files "src/**.cpp"

local
function widgetApp(name)
	project(name)
	kind "ConsoleApp"
	links { "widget", "glfw", "GL" }
end

widgetApp "example"
	files "example/Example.cpp"
	files {
		"include/widget/**.cpp",
		"src/**.cpp",
		"example/**.cpp"
	}
	links { "glfw", "GL", "freetype" }
	includedirs {
		"external/stx/include",
		"external/stxmath/include",
		"external/freetype2/include"
	}
