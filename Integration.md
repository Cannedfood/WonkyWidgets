# Integration

- Build the library (See section in [Readme](Readme.md)) and link it against your project

- Choose:
	1. Only use the libraries' Window class and OpenGL1
	```C++
	#include <widget/Window.hpp>
	#include <widget/widget/Button.hpp>
	void main() {
		using namespace wwidget;
		Window window;
		window.open("Title", 800, 600, Window::FlagAntialias | Window::FlagVsync);

		Button button(&window, "Click.Me.", [](Button& b) {
			b.findParent<Window>()->popup("Thank you.");
		});

		while(window.update()) {
			window.draw();
		}
	}
	```
	2. Use one of the inline Canvas implementations in include/wwidget/platform
	```C++
	#include <widget/platform/OpenGL1.hpp>
	#include <widget/widget/Button.hpp>
	void main() {
		using namespace wwidget;

		OpenGL1_Canvas canvas;

		Button button(&root, "Click.Me.", [](Button& b) {
			b.findParent<Window>()->popup("Thank you.");
		});

		button.draw(canvas);
	}
	```
	3. Roll your own Canvas and Bitmap classes
		- Directly on the Canvas interface
		```C++
		#include <widget/Canvas.hpp>
		class MyCanvas : public Canvas {
			std::shared_ptr<Bitmap> loadTextureNow(uint8_t* data, unsigned w, unsigned h, unsigned components) override;

			// Act as if the canvas was this size until popArea is called.
			// In OpenGL1 this would be the same as glPushMatrix(); glTranslate(x, y, 0);
			void pushArea(float x, float y, float w, float h) override;
			void popArea() override;

			// All units are in pixels (including texcoords)
			void outlinePoly(float* points, size_t number, uint32_t color)  override;
			void fillPoly(float* points, size_t number, uint32_t color)  override;
			void fillPoly(float* points, float* texcoords, size_t number, Bitmap* bitmap, uint32_t tint)  override;
		};
		```
		- Using buffers
		```C++
		#include <widget/platform/BufferedCanvas>
		class OpenGL3_Canvas : public BufferedCanvas {
			std::shared_ptr<Bitmap> loadTextureNow(uint8_t* data, unsigned w, unsigned h, unsigned components) override;

			void flush() {
				RenderBuffer buffer;
				while(popBuffer(&buffer)) {
					// Draw the buffer
				}
			}
		};
		```
