#ifndef WWIDGET_EXTERNAL_NANOVG
	extern "C" {
		#include "nanovg.inl"
	}
#elif !defined(WWIDGET_EXTERNAL_STBI)
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
#endif
