#include "../include/widget/Window.hpp"

#ifndef WIDGET_NO_WINDOWS

#define WIDGET_OPENGL1_IMPLEMENTATION
#include "../include/widget/platform/OpenGL1.hpp"
#include "../include/widget/platform/OpenGL1_3D.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include "3rd-party/tinyformat.h"

#define mWindow ((GLFWwindow*&) mWindowPtr)

namespace widget {

static int gNumWindows = 0;

static
void myGlfwErrorCallback(int level, const char* msg) {
	tfm::vformat(std::cerr, "GLFW: (%i): %s", tfm::makeFormatList(level, msg));
}

static
void myGlfwWindowResized(GLFWwindow* win, int width, int height) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	window->size(width, height);

	glfwMakeContextCurrent(win);
	glViewport(0, 0, width, height);
}

static
void myGlfwWindowPosition(GLFWwindow* win, int x, int y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	if(window->relative()) {
		window->position(x, y);
	}
}

static
void myGlfwWindowIconify(GLFWwindow* win, int iconified) {
	// Window* window = (Window*) glfwGetWindowUserPointer(win);
	int w, h;
	glfwGetWindowSize(win, &w, &h);
	myGlfwWindowResized(win, w, h);
}

static
void myGlfwCursorPosition(GLFWwindow* win, double x, double y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	window->mouse().x = x + window->area().x;
	window->mouse().y = y + window->area().y;
}

static
void myGlfwClick(GLFWwindow* win, int button, int action, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	Click click;
	click.x      = window->mouse().x;
	click.y      = window->mouse().y;
	click.button = button;
	switch (action) {
		case GLFW_RELEASE: click.state = Click::UP; break;
		case GLFW_PRESS:   click.state = Click::DOWN; break;
		case GLFW_REPEAT:  click.state = Click::DOWN_REPEATING; break;
	}
	window->send(click);
}

Window::Window() :
	mWindowPtr(nullptr)
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
	glfwWindowHint(     GLFW_SAMPLES,   (flags & FlagAntialias) ? 4 : 0);
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

bool Window::update() {
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

void Window::draw() {
	glfwMakeContextCurrent(mWindow);

	Widget::draw(*mCanvas);
}

void Window::onDraw(Canvas& canvas) {
	glfwSwapBuffers(mWindow);
}

} // namespace widget

#endif // defined(WIDGET_NO_WINDOWS)
