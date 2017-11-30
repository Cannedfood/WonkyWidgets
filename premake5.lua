workspace "widget"

language "C++"
cppdialect "C++17"

configurations {
	"debug",
	"release"
}

project "widget"
	kind "ConsoleApp"
	symbols "On"
	-- defines {
	-- 	"WIDGET_ULTRA_VERBOSE=1"
	-- }
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
