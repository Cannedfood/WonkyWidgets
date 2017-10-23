#include "Window.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include <xlogging>

#define mWindow ((GLFWwindow*&) mWindowPtr)

namespace widget {

static int gNumWindows = 0;

static
void myGlfwErrorCallback(int level, const char* msg) {
	stx::warn("GLFW: (", level, "): ", msg);
}

static
void myGlfwWindowResized(GLFWwindow* win, int width, int height) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	window->area().width  = width;
	window->area().height = height;
	window->area().uwidth  = PX;
	window->area().uheight = PX;

	glfwMakeContextCurrent(win);
	glViewport(0, 0, width, height);
}

static
void myGlfwWindowPosition(GLFWwindow* win, int x, int y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	window->area().x = x;
	window->area().y = y;
	window->area().ux = PX;
	window->area().uy = PX;
}

static
void myGlfwCursorPosition(GLFWwindow* win, double x, double y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	window->mouse().x = x + window->area().x;
	window->mouse().y = y + window->area().y;
}

static
void myGlfwClick(GLFWwindow* win, int button, int action, int mods) {
	if(action <= 0) return;
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	Click click;
	click.x      = window->mouse().x;
	click.y      = window->mouse().y;
	click.button = button;
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
			"Window::open(" + std::string(title) + ", " + std::to_string(width) + ", " + std::to_string(height) + ")"
		);
	}

	if(gNumWindows <= 0) {
		if(!glfwInit()) {
			throw std::runtime_error("Failed initializing glfw");
		}
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_DOUBLEBUFFER, ((flags & DOUBLEBUFFERED) != 0));
	mWindow = glfwCreateWindow(width, height, title, NULL, NULL);
	if(!mWindow) {
		if(gNumWindows <= 0) {
			glfwTerminate();
			glfwSetErrorCallback(myGlfwErrorCallback);
		}

		throw exceptions::FailedOpeningWindow(
			"Window::open(" + std::string(title) + ", " + std::to_string(width) + ", " + std::to_string(height) + ")"
		);
	}
	glfwMakeContextCurrent(mWindow);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetWindowSizeCallback(mWindow, myGlfwWindowResized);
	glfwSetWindowPosCallback(mWindow, myGlfwWindowPosition);

	glfwSetCursorPosCallback(mWindow, myGlfwCursorPosition);
	glfwSetMouseButtonCallback(mWindow, myGlfwClick);

	glfwSwapInterval((flags & VSYNC) ? -1 : 0);

	area() = Area(width, height, PX);

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

bool Window::update() {
	glfwWaitEvents();
	return !glfwWindowShouldClose(mWindow);
}

void Window::draw() {
	// Frame::draw();
	glfwSwapBuffers(mWindow);
}

} // namespace widget
