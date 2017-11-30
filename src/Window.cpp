#include "../include/widget/Window.hpp"

#ifndef WIDGET_NO_WINDOWS

#define WIDGET_OPENGL1_IMPLEMENTATION
#include "../include/widget/platform/OpenGL1.hpp"
#include "../include/widget/platform/OpenGL1_3D.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#define mWindow ((GLFWwindow*&) mWindowPtr)

namespace widget {

static int gNumWindows = 0;

static
void myGlfwErrorCallback(int level, const char* msg) { WIDGET_FN_MARKER
	std::cerr << "GLFW: (" << level << "): " << msg << std::endl;
}

static
void myGlfwWindowResized(GLFWwindow* win, int width, int height) { WIDGET_FN_MARKER
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	if(window->hasConstantSize())
		window->size(width, height);

	glfwMakeContextCurrent(win);
	glViewport(0, 0, width, height);
}

static
void myGlfwWindowPosition(GLFWwindow* win, int x, int y) { WIDGET_FN_MARKER
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	if(window->relative()) {
		window->offset(x, y);
	}
}

static
void myGlfwWindowIconify(GLFWwindow* win, int iconified) { WIDGET_FN_MARKER
	// Window* window = (Window*) glfwGetWindowUserPointer(win);
	int w, h;
	glfwGetWindowSize(win, &w, &h);
	myGlfwWindowResized(win, w, h);
}

static
void myGlfwCursorPosition(GLFWwindow* win, double x, double y) { WIDGET_FN_MARKER
	Window* window = (Window*) glfwGetWindowUserPointer(win);

	Dragged drag;
	drag.buttons = window->mouse().buttons;
	drag.old_x   = window->mouse().x;
	drag.old_y   = window->mouse().y;
	drag.x       = window->mouse().x = x + window->offsetx();
	drag.y       = window->mouse().y = y + window->offsety();
	drag.moved_x = drag.x - drag.old_x;
	drag.moved_y = drag.y - drag.old_y;

	if(window->mouse().buttons.any()) {
		window->send(drag);
	}
	else {
		window->send((Moved&)drag);
	}
}

static
void myGlfwClick(GLFWwindow* win, int button, int action, int mods) { WIDGET_FN_MARKER
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	Click click;
	click.x      = window->mouse().x;
	click.y      = window->mouse().y;
	click.button = button;
	window->mouse().buttons[button] = action != GLFW_RELEASE;
	switch (action) {
		case GLFW_RELEASE: click.state = Click::UP; break;
		case GLFW_PRESS:   click.state = Click::DOWN; break;
		case GLFW_REPEAT:  click.state = Click::DOWN_REPEATING; break;
	}
	window->send(click);
}

static
void myGlfwScroll(GLFWwindow* win, double x, double y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);

	Scroll scroll;
	scroll.x       = window->mouse().x;
	scroll.y       = window->mouse().y;
	// TODO: doesn't scale with dpi
	scroll.clicks_x = (float) x;
	scroll.clicks_y = (float) y;
	scroll.pixels_x = scroll.clicks_x * 16;
	scroll.pixels_y = scroll.clicks_y * 16;
	window->send(scroll);
}

Window::Window() :
	mWindowPtr(nullptr),
	mFlags(0)
{}

Window::Window(const char* title, unsigned width, unsigned height, uint32_t flags) :
	Window()
{
	open(title, width, height, flags);
}

Window::~Window() {
	close();
}

void Window::open(const char* title, unsigned width, unsigned height, uint32_t flags) {
	if(mWindow) {
		throw std::runtime_error(
			"Window already opened."
			"Window::open(\"" + std::string(title) + "\", " + std::to_string(width) + ", " + std::to_string(height) + ", " + std::to_string(flags) + ")"
		);
	}

	if(gNumWindows <= 0) {
		if(!glfwInit()) {
			throw std::runtime_error("Failed initializing glfw");
		}
		glfwSetErrorCallback(myGlfwErrorCallback);
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_DOUBLEBUFFER, ((flags & FlagDoublebuffered) != 0));
	glfwWindowHint(     GLFW_SAMPLES,  (flags & FlagAntialias) ? 4 : 0);
	glfwWindowHint(GLFW_RESIZABLE, (flags & FlagConstantSize) != 0);
	mRelative = flags & FlagRelative;
	mWindow   = glfwCreateWindow(width, height, title, NULL, NULL);
	if(!mWindow) {
		if(gNumWindows <= 0) {
			glfwTerminate();
			glfwSetErrorCallback(myGlfwErrorCallback);
		}

		throw exceptions::FailedOpeningWindow(
			"Window::open(\"" + std::string(title) + "\", " + std::to_string(width) + ", " + std::to_string(height) + ", " + std::to_string(flags) + ")"
		);
	}
	size(width, height);
	glfwMakeContextCurrent(mWindow);
	glfwSetWindowUserPointer(mWindow, this);

	glfwSetFramebufferSizeCallback(mWindow, myGlfwWindowResized);
	glfwSetWindowPosCallback(mWindow, myGlfwWindowPosition);
	glfwSetWindowIconifyCallback(mWindow, myGlfwWindowIconify);

	glfwSetCursorPosCallback(mWindow, myGlfwCursorPosition);
	glfwSetMouseButtonCallback(mWindow, myGlfwClick);
	glfwSetScrollCallback(mWindow, myGlfwScroll);

	glfwSwapInterval((flags & FlagVsync) ? -1 : 0);

	mFlags = flags;

	// TODO: don't ignore FlagAnaglyph3d
	mCanvas = std::make_shared<OpenGL1_Canvas>();

	++gNumWindows;
}

void Window::close() {
	if(mWindow) {
		glfwDestroyWindow(mWindow);
		--gNumWindows;
		if(gNumWindows <= 0) {
			glfwTerminate();
		}
	}
}

void Window::requestClose() {
	if(mWindow) {
		glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
	}
}

bool Window::update() { WIDGET_M_FN_MARKER
	if(mFlags & FlagUpdateOnEvent)
		glfwWaitEvents();
	else
		glfwPollEvents();

	return !glfwWindowShouldClose(mWindow);
}

void Window::keepOpen() {
	while(update()) {
		draw();
	}
}

void Window::onCalculateLayout(LayoutInfo& info) {
	if(mFlags & FlagConstantSize) {
		int w, h;
		glfwGetWindowSize(mWindow, &w, &h);
		info.prefx = info.minx = info.maxx = w;
		info.prefy = info.miny = info.maxy = h;
	}
	else {
		Widget::onCalculateLayout(info);
	}
}

void Window::onResized() {
	if(!hasConstantSize()) {
		glfwSetWindowSize(mWindow, width(), height());
	}
	requestRelayout();
}

void Window::draw() {
	glfwMakeContextCurrent(mWindow);

	Widget::draw(*mCanvas);
}

void Window::onDrawBackground(Canvas& canvas) {
	canvas.fillRect(0, 0, width(), height(), rgb(41, 41, 41));
}

void Window::onDraw(Canvas& canvas) {
	if(mFlags & FlagDrawDebug) {
		auto drawLayoutInfos = [this](auto& recurse, Widget* w) -> void {
			w->eachChild([&](Widget* c) {
				mCanvas->pushArea(c->offsetx(), c->offsety(), c->width(), c->height());
				{
					LayoutInfo info;
					c->getLayoutInfo(info);
					mCanvas->outlineRect(0, 0, c->width(), c->height(), rgb(219, 0, 255));
					mCanvas->outlineRect(0, 0, info.minx,  info.miny,  rgba(255, 0, 0, 0.5f));
					mCanvas->fillRect(0, 0, info.prefx, info.prefy, rgba(0, 255, 0, 0.1f));
					mCanvas->outlineRect(0, 0, info.maxx,  info.maxy,  rgba(0, 0, 255, 0.5f));
				}
				recurse(recurse, c);
				mCanvas->popArea();
			});
		};
		drawLayoutInfos(drawLayoutInfos, this);
	}

	glfwSwapBuffers(mWindow);
}

} // namespace widget

#endif // defined(WIDGET_NO_WINDOWS)
