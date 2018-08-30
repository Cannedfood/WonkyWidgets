#include "../include/wwidget/Window.hpp"

#include "../include/wwidget/CanvasNVG.cpp"

#include <GLFW/glfw3.h>

// #include <stx/timer.hpp>


#define NANOVG_GL3_IMPLEMENTATION

static PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
static PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
static PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
static PFNGLCREATEPROGRAMPROC            glCreateProgram;
static PFNGLCREATESHADERPROC             glCreateShader;
static PFNGLSHADERSOURCEPROC             glShaderSource;
static PFNGLCOMPILESHADERPROC            glCompileShader;
static PFNGLGETSHADERIVPROC              glGetShaderiv;
static PFNGLATTACHSHADERPROC             glAttachShader;
static PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
static PFNGLLINKPROGRAMPROC              glLinkProgram;
static PFNGLGETPROGRAMIVPROC             glGetProgramiv;
static PFNGLDELETEPROGRAMPROC            glDeleteProgram;
static PFNGLDELETESHADERPROC             glDeleteShader;
static PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
static PFNGLGETUNIFORMBLOCKINDEXPROC     glGetUniformBlockIndex;
static PFNGLGENBUFFERSPROC               glGenBuffers;
static PFNGLUNIFORMBLOCKBINDINGPROC      glUniformBlockBinding;
static PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;
static PFNGLBINDBUFFERRANGEPROC          glBindBufferRange;
static PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
static PFNGLUSEPROGRAMPROC               glUseProgram;
static PFNGLBINDBUFFERPROC               glBindBuffer;
static PFNGLBUFFERDATAPROC               glBufferData;
static PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
static PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
static PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
static PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
static PFNGLUNIFORM1IPROC                glUniform1i;
static PFNGLUNIFORM2FVPROC               glUniform2fv;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
static PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
static PFNGLDELETEBUFFERSPROC            glDeleteBuffers;


#include <nanovg_gl.h>

#include <stdexcept>
#include <iostream>

#define mWindow ((GLFWwindow*&) mWindowPtr)

namespace wwidget {

static int gNumWindows = 0;

static
void myGlfwErrorCallback(int level, const char* msg) {
	std::cerr << "GLFW: (" << level << "): " << msg << std::endl;
}

static
void myGlfwWindowResized(GLFWwindow* win, int width, int height) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	if(!window->doesShrinkFit())
		window->size(width, height);

	glfwMakeContextCurrent(win);
	glViewport(0, 0, width, height);
}

static
void myGlfwWindowPosition(GLFWwindow* win, int x, int y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	// TODO: make this trigger an event
	if(window->relative()) {
		window->offset(-x, -y);
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

	Dragged drag;
	drag.buttons    = window->mouse().buttons;
	drag.old_x      = window->mouse().x;
	drag.old_y      = window->mouse().y;
	drag.position.x = window->mouse().x = x + window->offsetx();
	drag.position.y = window->mouse().y = y + window->offsety();
	drag.moved_x    = drag.position.x - drag.old_x;
	drag.moved_y    = drag.position.y - drag.old_y;

	if(window->mouse().buttons.any()) {
		window->send(drag);
	}
	else {
		window->send((Moved&)drag);
	}
}

static
void myGlfwClick(GLFWwindow* win, int button, int action, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);
	Click click;
	click.position = {window->mouse().x, window->mouse().y};
	click.button = button;
	window->mouse().buttons[button] = action != GLFW_RELEASE;
	switch (action) {
		case GLFW_RELEASE: click.state = Event::UP; break;
		case GLFW_PRESS:   click.state = Event::DOWN; break;
		case GLFW_REPEAT:  click.state = Event::DOWN_REPEATING; break;
	}
	window->send(click);
}

static
void myGlfwScroll(GLFWwindow* win, double x, double y) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);

	Scroll scroll;
	scroll.position = {window->mouse().x, window->mouse().y};
	// TODO: doesn't scale with dpi
	scroll.clicks_x = (float) x;
	scroll.clicks_y = (float) y;
	scroll.pixels_x = scroll.clicks_x * 48;
	scroll.pixels_y = scroll.clicks_y * 48;
	window->send(scroll);
}

static
void myGlfwKeyInput(GLFWwindow* win, int key, int scancode, int action, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);

	KeyEvent k;
	k.position.x = window->mouse().x;
	k.position.y = window->mouse().y;
	switch (action) {
		case GLFW_RELEASE: k.state = Event::UP; break;
		case GLFW_PRESS:   k.state = Event::DOWN; break;
		case GLFW_REPEAT:  k.state = Event::DOWN_REPEATING; break;
	}
	k.mods     = mods;
	k.key      = key;
	k.scancode = scancode;
	window->send(k);
}

static
void myGlfwCharInput(GLFWwindow* win, unsigned int codepoint, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(win);

	TextInput t;
	t.mods = mods;
	t.position.x    = window->mouse().x;
	t.position.y    = window->mouse().y;

	t.utf32 = codepoint;
	t.calcUtf8();
	window->send(t);
}


Window::Window() :
	mWindowPtr(nullptr),
	mFlags(0)
{
	BasicContext::rootWidget(this);
}

Window::Window(const char* title, unsigned width, unsigned height, uint32_t flags) :
	Window()
{
	open(title, width, height, flags);
}

Window::~Window() {
	clearChildren();
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
	glfwWindowHint(GLFW_DOUBLEBUFFER, ((flags & FlagSinglebuffered) == 0));
	// glfwWindowHint(     GLFW_SAMPLES,  (flags & FlagAntialias) ? 4 : 0);
	glfwWindowHint(GLFW_RESIZABLE, (flags & FlagShrinkFit) == 0);

	{
		GLFWvidmode const* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
		glfwWindowHint(GLFW_ALPHA_BITS, 0);
		glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);

		glfwWindowHint(GLFW_ACCUM_RED_BITS, 0);
		glfwWindowHint(GLFW_ACCUM_GREEN_BITS, 0);
		glfwWindowHint(GLFW_ACCUM_BLUE_BITS, 0);
		glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 0);
		glfwWindowHint(GLFW_DEPTH_BITS, 0);
		glfwWindowHint(GLFW_STENCIL_BITS, 0);
	}

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
	glfwSetKeyCallback(mWindow, myGlfwKeyInput);
	glfwSetCharModsCallback(mWindow, myGlfwCharInput);

	glfwSwapInterval((flags & FlagNoVsync) == 0 ? 1 : 0);

	mFlags = flags;

	#define GLPROC(NAME) NAME = reinterpret_cast<decltype(NAME)>(glfwGetProcAddress(#NAME))
	GLPROC(glBlendFuncSeparate);
	GLPROC(glGetShaderInfoLog);
	GLPROC(glGetProgramInfoLog);
	GLPROC(glCreateProgram);
	GLPROC(glCreateShader);
	GLPROC(glShaderSource);
	GLPROC(glCompileShader);
	GLPROC(glGetShaderiv);
	GLPROC(glAttachShader);
	GLPROC(glBindAttribLocation);
	GLPROC(glLinkProgram);
	GLPROC(glGetProgramiv);
	GLPROC(glDeleteProgram);
	GLPROC(glDeleteShader);
	GLPROC(glGetUniformLocation);
	GLPROC(glGetUniformBlockIndex);
	GLPROC(glGenBuffers);
	GLPROC(glUniformBlockBinding);
	GLPROC(glGenerateMipmap);
	GLPROC(glBindBufferRange);
	GLPROC(glStencilOpSeparate);
	GLPROC(glUseProgram);
	GLPROC(glBindBuffer);
	GLPROC(glBufferData);
	GLPROC(glBindVertexArray);
	GLPROC(glGenVertexArrays);
	GLPROC(glEnableVertexAttribArray);
	GLPROC(glVertexAttribPointer);
	GLPROC(glUniform1i);
	GLPROC(glUniform2fv);
	GLPROC(glDisableVertexAttribArray);
	GLPROC(glDeleteVertexArrays);
	GLPROC(glDeleteBuffers);
	#undef GLPROC

	// TODO: don't ignore FlagAnaglyph3d
	canvas(make_shared<CanvasNVG>(nvgCreateGL3((flags & FlagAntialias) ? NVG_ANTIALIAS : 0), nvgDeleteGL3));

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

	BasicContext::update();

	return !glfwWindowShouldClose(mWindow);
}

void Window::keepOpen() {
	while(update()) {
		draw();
	}
}

PreferredSize Window::onCalcPreferredSize(PreferredSize const& constraint) {
	if(doesShrinkFit())
		return Widget::onCalcPreferredSize(constraint);

	PreferredSize result;
	int w, h;
	glfwGetWindowSize(mWindow, &w, &h);
	result.pref.x = result.min.x = result.max.x = w;
	result.pref.y = result.min.y = result.max.y = h;

	return result;
}

void Window::onResized() {
	if(doesShrinkFit()) {
		glfwSetWindowSize(mWindow, width(), height());
	}
	preferredSizeChanged();
	requestRelayout();
}

void Window::draw() {
	glfwMakeContextCurrent(mWindow);

	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BasicContext::draw();

	glfwSwapBuffers(mWindow);
}

void Window::onDrawBackground(Canvas& c) {
	c.fillColor(rgb(41, 41, 41))
	 .rect(size())
	 .fill();
}

void Window::onDraw(Canvas& canvas) {
	if(mFlags & FlagDrawDebug) {
		// TODO: debug draw
	}
}

} // namespace wwidget
