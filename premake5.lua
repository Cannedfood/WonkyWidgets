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

if _OPTIONS.cc == 'clang' or _OPTIONS.cc == 'gcc' then
	filter 'configurations:dev or debug'
		buildoptions {
			'-Wall', '-Wextra',
			'-Wno-unused-parameter'
		}
	filter {}
end

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
