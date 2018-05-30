#ifndef WWIDGET_NO_WINDOWS
	#ifdef WWIDGET_X11
		#include "WindowImpl/WindowX11.inl" // Xorg implementation
	#elif defined(WWIDGET_DIRECT2D)
		#include "WindowImpl/WindowDirect2D.inl" // Direct2D (Windows) implementation
	#else
		#include "WindowImpl/WindowGLFW.inl" // Cross-platform implementation, depends on GLFW
	#endif
#endif // ifndef WWIDGET_NO_WINDOWS