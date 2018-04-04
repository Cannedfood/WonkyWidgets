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
		includedirs "include"
end

widgetApp "example1"
	files "example/1-SimpleUi/**.cpp"
widgetApp "example3"
	files "example/3-Forms/**.cpp"

widgetApp "editor"
	files "example/editor/**.cpp"
