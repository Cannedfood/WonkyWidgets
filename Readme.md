# Widget toolkit thing

**Be careful when pulling from this repository. It is in beta and the API may change frequently. As a result the documentation can be out of date.**

This is a widget toolkit I created for my game engine.
(I will also write implementations for all OpenGL versions)

The goals are to be
- Easily embeddable in game engines
- Have controller support without having to do a lot of work
- Easy to build and dependency free (mostly)
- Elegant

The goals are *not*
- To fit into the desktop environment

## This library is WIP
### TODO:
#### API
- Make widgets more customizable
	- Add a "Style" class?
- Use nicer font rendering and text setting
	- Software render font to bitmaps
	- Add functionality for line breaks, following paths etc.
	- Allow the canvas to draw bitmap fonts directly
- Implement the better attribute API
	- Create a makro for the set function
	- Make most widgets move assignable and move constructable
	```C++
	Button myButton = {
		myButtonParent,
		"button text",
		Padding(10, 10),
		Align(Right)
	};
	myButton.set(
		Position{15, 15},
		Text{"Live long and prosper"}, //< Note: this is a full widget
		Image{"spock.png"}, //< Note: this is a full widget
		[]() { // On click
			puts("This isn't a high five.");
		}
	);
	```
- Add controller support
	- ?Flag wheather a widget is focusable by controller
- Update documentation
- Create widget layouts
	- Section (Collapsable list)
	- WrappedList (Basically a horizontal list with 'line breaks')
	- Table
- Create widgets
	- Color picker
	- File selector
	- File browser
	- Regex matching text field (Only allows content that matches the regex)
		- Number field
- Make compatible with MSVC and properly export classes (dllexport etc.)
- Optionally integrate with the desktop environment for dialogues
- Add a software renderer
	- Can be used for performance heavy drawing operations in another thread e.g.
		- Data visualization (Audio waveforms etc.)
		- High quality scaling
	- Also can be used to create a software-only Canvas implementation
- (Implement scripting? Probably lua?)
- Look forward to the C++ 2D graphics TS

#### Performance & Backend
- Make a Widgets smaller (as of writing a widget is 160 bytes large with clang, tendency growing)
	- Use TinyString wherever possible
	- Use packing where sensible
- Add partial redraws
- Add a FreeType font rendering backend
- Add a different file loading backend (stb has some issues, esp. with jpegs)

#### Bugs
- Sliders don't work properly with exponents other than 1
- Widgets don't clip children
- List and WrappedList doesn't balance the remaining space between children with AlignFill
- WrappedList doesn't fill gaps
- Padding is widely ignored

## Should I use this library
- **Caveat 0**: No. It is WIP and the documentation consists of *lies*. **LIEEES I SAY!**
- **Caveat 1**: It requires **C++17** or higher
- **Caveat 2**: It uses [RTTI](https://en.wikipedia.org/wiki/Run-time_type_information), so that's that. Feel free to implement a fallback ;)
- **Caveat 3**: It uses some exceptions.
  Mostly for:
  - Errors while loading files
  - Invalid operations and values (Adding a widget to itself, negative sizes etc.)
  - When you use a function that is supposed to
    - e.g. `Widget::find` throws an exception if a widget isn't found, in contrast to `Widget::search` which returns a `nullptr`

## Building

You can build it with [premake5](https://github.com/premake/premake-core/wiki) **or, preferably**, integrate it with your own build system:
You just have to build all the source files in `src/` and link with [glfw3](http://www.glfw.org) and OpenGL.

You can build this library with (almost) full functionality without any dependencies! (See list preprocessor definitions)

e.g. to build the examples
```bash
git clone --depth 1 https://github.com/Cannedfood/WonkyWidgets.git
cd WonkyWidgets
c++ `find ./src/ -name *.cpp` `find ./example/ -name *.cpp` -lglfw -lGL -lstdc++fs -lpthread --std=c++17 -o example.bin
./example.bin
```

On arch-like systems you can also install the library with makepkg (as wwidgets)
```
cd /tmp
mkdir wwidgets-git
cd wwidgets-git
curl https://raw.githubusercontent.com/Cannedfood/WonkyWidgets/master/PKGBUILD > PKGBUILD
makepkg --noconfirm -i
cd ..
rm -Rf wwidgets-git
```

List of preprocessor definitions:
- `WIDGET_NO_WINDOWS`: Disable the Window class. (Also removes the dependency on glfw3 and OpenGL.)
- ~~`WIDGET_ULTRA_VERBOSE`: Print a trace of all widget function calls to stdout while running~~ REMOVED
- ~~`WIDGET_USE_FREETYPE`: Use the freetype library for loading fonts. This enables:~~ TODO
	- Non-truetype fonts
	- Better font rendering
	- Less fonts that just generate segfaults (stb_truetype.h is, unfortunately, not perfect.)

## Minimal how to

~~You can find the "full" manual [here](Manual.md)~~

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
