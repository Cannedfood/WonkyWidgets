workspace "widget"

language "C++"
cppdialect "C++14"

configurations {
	"debug",
	"release"
}

project "widget"
	kind "ConsoleApp"
	symbols "On"
	files {
		"include/widget/**.cpp",
		"src/**.cpp",
		"test/**.cpp"
	}
	links {
		"glfw",
		"GL",
		"freetype"
	}
	includedirs {
		"external/stx/include",
		"external/stxmath/include",
		"external/freetype2/include"
	}