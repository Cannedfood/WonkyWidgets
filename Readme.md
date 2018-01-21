# Widget toolkit thing

**EVERYTHING IN THIS REPOSITORY IS WORK IN PROGRESS AND SHOULD NOT BE USED IN PRODUCTION CODE**

**DON'T TAKE THE DOCUMENTATION TOO SERIOUS, IT'S MORE A GUIDELINE HOW THE LIBRARY SHOULD WORK WHEN IT'S IN A RELEASABLE STATE**

This is a widget toolkit I created for my game engine.
(I will also write implementations for all OpenGL versions)

The goals are to be
- Easily embeddable in game engines
- Easy to build and dependency free (mostly)
- Elegant

## Should I use this library

Caveat 0: No. It is WIP and the documentation consists of *lies*. **LIEEES I SAY!** Meaning the api may change randomly at any time.

Caveat 1: It requires **C++14** or higher

Caveat 2: It uses [RTTI](https://en.wikipedia.org/wiki/Run-time_type_information), so that's that. Maybe I'll add a define at some point.

Caveat 3: It uses some exceptions.
Mostly for:
- Errors while loading files
- Invalid operations and values (Adding a widget to itself, negative sizes etc.)
- When you use a function that is supposed to
  - e.g. `Widget::find` throws an exception if a widget isn't found, in contrast to `Widget::search` which returns a `nullptr`

## Building

You can build it with [premake5](https://github.com/premake/premake-core/wiki) **or, preferably**, integrate it with your own build system:
You just have to build all the source files in `src/` and link with [glfw3](http://www.glfw.org) and OpenGL.

Don't like the dependency on glfw and/or OpenGL?
Define `WIDGET_NO_WINDOWS` when building the library.
You don't have to add the define to projects that use the library, it only affects source files.
e.g. to build the examples
```bash
git clone --depth 1 https://github.com/Cannedfood/WonkyWidgets.git
cd WonkyWidgets
c++ `find ./src/ -name *.cpp` `find ./example/ -name *.cpp` -lglfw -lGL --std=c++14 -o example.bin
./example.bin
```

On arch-like systems you can also install the library with makepkg (as wwidgets)
```
cd /tmp
mkdir wwidgets-git
cd wwidgets-git
curl https://raw.githubusercontent.com/Cannedfood/WonkyWidgets/master/PKGBUILD > PKGBUILD
makepkg --noconfirm -if
cd ..
rm -Rf wwidgets-git
```

List of preprocessor definitions:
- `WIDGET_NO_WINDOWS`: Disable the Window class. (Also removes the dependency on glfw3.)
- `WIDGET_ULTRA_VERBOSE`: Print a trace of all widget function calls to stdout while running
- ~~`WIDGET_USE_FREETYPE`: Use the freetype library for loading fonts. This enables:~~ TODO
	- Non-truetype fonts
	- Better font rendering
	- Less fonts that just generate segfaults (stb_truetype.h is, unfortunately, not perfect.)

## Minimal how to

You can find the "full" manual [here](Manual.md)

With C++ only (Objects on stack):
```c++
int main(int argc, const char** argv) {
	using namespace wwidget;

	Window window = {"MyTitle", 800, 600};
	List   layout = {window};

	Button b1 = {layout, "Like"};
	Button b2 = {layout, "Dislike", Padding(50, 50)};
}
```

With C++ only (Objects on heap, xml forms uses this):
```c++
int main(int argc, const char** argv) {
	using namespace wwidget;

	Window window = {"MyTitle", 800, 600};
	List*  layout = window.create<List>();

	Button* b1 = layout.create<Button>("Shiat!");
	Button* b2 = layout.create<Button>("Shiet!", Padding(50, 50));
}
```

With XML forms + C++:
```xml
<!-- In file Test.ui.xml -->
<form>
	<window title="Window1" width='800', height='600'>
		<list>
			<button name='b1'>
				Shiat!
			</button>
			<button name='b2' padding='50 50'>
				Shiet!
			</button>
		</list>
	</window>
</form>
```

```c++
int main(int argc, const char** argv) {
	using namespace wwidget;

	// Form is a widget that loads it's children from an xml file
	Form form("Test.ui.xml");

	// There are two functions to retrieve a widget:
	//   Widget::search and Widget::find.
	// They have the same syntax, but Widget::find throws an exception when
	// the widget isn't found

	// When no name is specified, Widget::find searches for the first object
	//  safely castable to the specified type (via dynamic_cast).
	Window* window = form.find<Window>();
	List*   list   = window->find<List>();
	//
	Button* b1     = list->find<Button>("b1");
	Button* b2     = list->find<Button>("b2");
}
```

# Todo:
- Make a basic Widget smaller (as of writing a widget is 160 bytes large with clang, tendency growing)
- Implement styles (Or however they will be called in the API)
- Cache LayoutInfos (Huge layout performance gain for bigger trees)
	- Also make it optional at compile time
- Make actual api conform to the documentation (or at least the other way around)
