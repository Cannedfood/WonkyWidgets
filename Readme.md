# Widget toolkit thing

**EVERYTHING IN THIS REPOSITORY IS WORK IN PROGRESS AND SHOULD NOT BE USED IN PRODUCTION CODE**

**DON'T TAKE THE DOCUMENTATION TOO SERIOUS, IT'S MORE A GUIDELINE HOW THE LIBRARY SHOULD WORK WHEN IT'S IN A RELEASABLE STATE**

This is a widget toolkit I created for my game engine.
As such it is easily embeddable with almost every graphics API.
(I will also write implementations for all OpenGL versions)

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
You just have to build all the source files in `src/` and link with [glfw3](http://www.glfw.org).

If you do not want the dependency to glfw3 or you don't use the libraries' Window class you can define `WIDGET_NO_WINDOWS`.
This definition only affects source files and therefore only has to be defined when building the library, not for all of your projects.

## Minimal how to

You can find the "full" manual [here](Manual.md)

With C++ only (Objects on stack):
```c++
int main(int argc, const char** argv) {
	using namespace widget;

	Window window = {"MyTitle", 800, 600};
	List   layout = {window};

	Button b1 = {layout, "Like"};
	Button b2 = {layout, "Dislike", Padding(50, 50)};
}
```

With C++ only (Objects on heap, xml forms uses this):
```c++
int main(int argc, const char** argv) {
	using namespace widget;

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
	using namespace widget;

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
