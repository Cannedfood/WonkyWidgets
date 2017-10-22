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
	glfwSwapInterval((flags & VSYNC) ? -1 : 0);

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
