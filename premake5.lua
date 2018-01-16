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

widgetApp "editor"
	files "editor/**.cpp"

widgetApp "example"
	files "example/Example.cpp"

project "chat"
	kind "StaticLib"
	files {
		"example/chat/Chat.cpp",
		"example/chat/socket.cpp"
	}

widgetApp "chat_ui"
	files "example/chat/ChatExample.cpp"
	links {"chat", "pthread"}

widgetApp "chat_cl"
	files "example/chat/ChatConsole.cpp"
	links {"chat", "pthread"}
