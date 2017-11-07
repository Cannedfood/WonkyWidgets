#include "../include/widget/Window.hpp"
#include "../include/widget/widget/Button.hpp"
#include "../include/widget/widget/Form.hpp"
#include "../include/widget/Font.hpp"

#include <cmath>

#include <GL/gl.h>

using namespace widget;

static void testWidgetTree();
static void testFont();
static void testUtility();

int main(int argc, char const** argv) {
	testWidgetTree();
	// testFont();
	// testUtility();

	Window window = { "Here goes your title", 800, 600, Window::FlagUpdateOnEvent };
	window.name("window");

	{
		Form form("test/test.form.xml");
		window.add(&form);
		form.extract();
	}

	while(window.update()) {
		window.draw();
	}

	return 0;
}

#define test(CONDITION) { if(!(CONDITION)) throw std::runtime_error("Line " + std::to_string(__LINE__) + ": Failed test " #CONDITION); }

static void testUtility() {
	test(Area("10 10") == Area(10, 10));
}

static void testFont() {
	Font font("/usr/share/fonts/TTF/DroidSans.ttf");
	return;

	constexpr unsigned w = 80, h = 80;
	std::vector<uint8_t> pixmap(w * h, 40);

	const char mapping[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
	for(size_t x = 0; x < w; x++) {
		for(size_t y = 0; y < h; y++) {
			unsigned idx = (pixmap[x + y * w] / 255.f) * sizeof(mapping);
			putchar(mapping[idx]);
		}
		putchar('\n');
	}
}

static void testWidgetDefault();
static void testWidgetAdd();
static void testWidgetRemove();
static void testWidgetInsertNextSibling();
static void testWidgetInsertPrevSibling();
static void testWidgetInsertAsParent();
static void testWidgetExtract();

static
void testWidgetTree() {
	testWidgetDefault();
	testWidgetAdd();
	testWidgetRemove();
	testWidgetExtract();
	testWidgetInsertNextSibling();
	testWidgetInsertPrevSibling();
	testWidgetInsertAsParent();
}



static
void testWidgetDefault() {
	Widget w;
	test(w.nextSibling() == nullptr);
	test(w.prevSibling() == nullptr);
	test(w.parent()      == nullptr);
	test(w.children()    == nullptr);
}

static
void testWidgetAdd() {
	Widget w0, w1, w2, w3;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);

	test(w0.children() == &w1);
	test(w1.parent() == &w0);
	test(w2.parent() == &w0);
	test(w3.parent() == &w0);

	test(w3.nextSibling() == nullptr);
	test(w3.prevSibling() == &w2);
	test(w2.nextSibling() == &w3);
	test(w2.prevSibling() == &w1);
	test(w1.nextSibling() == &w2);
	test(w1.prevSibling() == nullptr);
}

static
void testWidgetRemove() {
	Widget w0, w1, w2, w3;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);


	w2.remove();
	test(w2.parent()      == nullptr);
	test(w2.prevSibling() == nullptr);
	test(w2.nextSibling() == nullptr);
	test(w3.prevSibling() == &w1);
	test(w1.nextSibling() == &w3);
	test(w3.nextSibling() == nullptr);
	test(w1.prevSibling() == nullptr);

	w1.remove();
	test(w1.parent()      == nullptr);
	test(w1.nextSibling() == nullptr);
	test(w1.prevSibling() == nullptr);
	test(w3.prevSibling() == nullptr);
	test(w3.nextSibling() == nullptr);
	test(w0.children()    == &w3);
}

static
void testWidgetInsertNextSibling() {
	Widget w0;
	Widget w1, w2, w3;
	Widget w;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);

	w2.insertNextSibling(&w);

	test(w.parent() == &w0);
	test(w2.nextSibling() == &w);
	test(w3.prevSibling() == &w);
	test(w.nextSibling()  == &w3);
	test(w.prevSibling()  == &w2);
}

static
void testWidgetInsertPrevSibling() {
	Widget w0;
	Widget w1, w2, w3;
	Widget w;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);

	w2.insertPrevSibling(&w);

	test(w.parent() == &w0);
	test(w1.nextSibling() == &w);
	test(w2.prevSibling() == &w);
	test(w.nextSibling()  == &w2);
	test(w.prevSibling()  == &w1);
}

static
void testWidgetInsertAsParent() {
	Widget w0;
	Widget w1, w2, w3;
	Widget w;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);

	w2.insertAsParent(&w);

	test(w2.parent()  == &w);
	test(w.children() == &w2);

	test(w1.nextSibling() == &w);
	test(w.prevSibling()  == &w1);

	test(w3.prevSibling() == &w);
	test(w.nextSibling() == &w3);
}

static
void testWidgetExtract() {
	Widget w0, w1, w2, w3, wa, wb, wc;

	w0.add(&w3);
	w0.add(&w2);
	w0.add(&w1);

	w2.add(&wc);
	w2.add(&wb);
	w2.add(&wa);


	w2.extract();

	test(w2.nextSibling() == nullptr);
	test(w2.prevSibling() == nullptr);
	test(w2.parent()      == nullptr);
	test(w2.children()    == nullptr);

	test(wa.parent() == &w0);
	test(wb.parent() == &w0);
	test(wc.parent() == &w0);

	test(wa.prevSibling() == &w1);
	test(w1.nextSibling() == &wa);

	test(wc.nextSibling() == &w3);
	test(w3.prevSibling() == &wc);
}
