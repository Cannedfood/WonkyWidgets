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

if _OPTIONS.cc == 'clang' or _OPTIONS.cc == 'gcc' then
	buildoptions {
		'-Wall', '-Wextra',
		'-Wno-unused-parameter',
		'-Wno-unused-function'
	}
end

optimize "Debug"
symbols "On"

project "wwidget"
	kind "StaticLib"
	files "src/**.cpp"

local
function widgetApp(name)
	project(name)
		kind "ConsoleApp"
		links { "wwidget", "glfw", "GL", "stdc++fs", "pthread" }
		includedirs "include"
end

widgetApp "unittests"
	files "example/unittests/**.cpp"

widgetApp "example1"
	files "example/1-SimpleUi/**.cpp"
widgetApp "example2"
	files "example/2-SimpleApp/**.cpp"

widgetApp "editor"
	files "example/editor/**.cpp"
