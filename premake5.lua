workspace 'WonkyWidgets'

language   'C++'
cppdialect 'C++17'

configurations {
	'dev',
	'debug',
	'release',
}

filter 'configurations:release or dev'
	optimize 'Speed'
filter 'configurations:dev or debug'
	symbols 'On'
filter 'configurations:debug'
	optimize 'Debug'
filter 'configurations:release'
	flags 'LinkTimeOptimization'
filter {}

filter { 'configurations:dev or debug', 'toolset:gcc or clang' }
	buildoptions { '-Wall', '-Wextra', '-Wno-unused-parameter' }
filter {}

flags { 'MultiProcessorCompile', 'NoIncrementalLink' }
vectorextensions 'SSE2'
floatingpoint 'Fast'


includedirs {
	"external/stx/include",
	"external/stxmath/include",
	"external/freetype2/include"
}

project "wwidget"
	kind "StaticLib"
	files "src/**.cpp"

project "nanovg"
	kind "StaticLib"
	files "thirdparty/nanovg/src/nanovg.c"

local
function widgetApp(name)
	project(name)
		kind "ConsoleApp"
		links { "wwidget", "glfw", "GL", "stdc++fs", "pthread", "nanovg" }
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
