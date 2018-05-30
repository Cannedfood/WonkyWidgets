#ifndef WWIDGET_NO_WINDOWS
	#ifdef WWIDGET_X11
		#include "WindowX11.inl" // Xorg implementation
	#elif defined(WWIDGET_DIRECT2D)
		#include "WindowDirect2D.inl" // Direct2D (Windows) implementation
	#else
		#include "WindowGLFW.inl" // Cross-platform implementation, depends on GLFW
	#endif
#endif // ifndef WWIDGET_NO_WINDOWS