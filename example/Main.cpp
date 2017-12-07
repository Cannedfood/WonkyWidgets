#include "../include/widget/Window.hpp"
#include "../include/widget/debug/Dump.hpp"
#include "../include/widget/widget/Button.hpp"
#include "../include/widget/widget/List.hpp"
#include "../include/widget/widget/ProgressBar.hpp"
#include "../include/widget/widget/Slider.hpp"
#include "../include/widget/widget/Form.hpp"
#include "../include/widget/platform/BufferedCanvas.hpp"
#include "../include/widget/widget/Dialogue.hpp"

#include <cmath>
#include <fstream>

#include <GL/gl.h>

using namespace widget;
using namespace std;

static void testWidgetTree();
static void testFont();
static void testUtility();

int main(int argc, char const** argv) {
	testWidgetTree();
	// testFont();
	// testUtility();

	WIDGET_ENABLE_MARKERS

	Window window = {
		"Here goes your title", 800, 600,
		0
		| Window::FlagUpdateOnEvent
		| Window::FlagAntialias
		| Window::FlagConstantSize
		// | Window::FlagDrawDebug
	};

	Form form("example/test.form.xml");
	window.add(&form);

	window.find<Button>("btnSuicide")->onClickCallback = [](auto* b) {
		if(b->text() != "Are you sure? [Yes]")
			b->text("Are you sure? [Yes]");
		else
			b->remove();
	};

	window.find<Button>("btnOrientation")->onClickCallback = [](Button* b) {
		List* l;
		l = b->findParent<List>("listMain");
		l->flow((widget::List::Flow)(l->flow() ^ List::FlowHorizontalBit));
		l = b->findParent<List>();
		l->flow((widget::List::Flow)(l->flow() ^ List::FlowHorizontalBit));
		l->align(Widget::AlignCenter, Widget::AlignCenter);
	};
	window.find<Button>("btnDirection")->onClickCallback = [](Button* b) {
		List* l;
		l = b->findParent<List>("listMain");
		l->flow((widget::List::Flow)(l->flow() ^ List::FlowNegativeBit));
		l = b->findParent<List>();
		l->flow((widget::List::Flow)(l->flow() ^ List::FlowNegativeBit));
	};

	window.find<Button>("btnClickerGame")->onClickCallback = [](Button* b) {
		auto* p = b->find<ProgressBar>();
		p->progress(p->progress() + 1);
		if(p->progress() > p->scale()) {
			p->progress(p->progress() - p->scale());
			p->scale(p->scale() * 1.2f);
		}
	};

	window.find<Slider>("sldTest")->valueCallback = [](Slider* s, float v) {
		std::string c = std::to_string(v);
		while(c.front() == '0') c = c.substr(1);
		while(c.back()  == '0' && c.size() > 2) c.pop_back();
		s->find<Label>()->content(c);
	};

	window.find<Button>("btnTestConfirm")
		->onClick([](Button* b) {
			Widget* r = b->findRoot();
			r->add<ConfirmationDialogue>("Did you want to open this?", [=]() {
				if(b) {
					r->add<ConfirmationDialogue>("You intentionally opened a dialogue!");
				}
			}, nullptr);
		});

	List dumpList;
	dumpList.offset(400, 0);

	window.find<Button>("btnDumpState")->onClickCallback = [&](Button* b) {
		if(auto file = std::ofstream("structure.tmp.txt"))
			PrintDump(file, &form);
		ShowDump(&window, &form)->offset(600, 0)->align(Widget::AlignMax, Widget::AlignMin);
	};

	window.add(dumpList);

#ifdef WIDGET_ULTRA_VERBOSE
	puts("Starting loop");
#endif

	window.keepOpen();

#ifdef WIDGET_ULTRA_VERBOSE
	puts("Ended loop");
#endif

	return 0;
}

#define test(CONDITION) { if(!(CONDITION)) throw std::runtime_error("Line " + std::to_string(__LINE__) + ": Failed test " #CONDITION); }

static void testUtility() {
	// test(Area("10 10") == Area(10, 10));
}

#include "../include/widget/Font.hpp"
#include "../include/widget/fonts/FontDescription.hpp"

#include <fstream>

static void testFont() {
	Font font("/usr/share/fonts/TTF/DejaVuSerif.ttf");
	shared_ptr<BitmapFont> bmf = font.get(32);

	if(auto file = std::ofstream("./test.txt")) {
		for(size_t y = 0; y < bmf->height(); y++) {
			for(size_t x = 0; x < bmf->width(); x++) {
				size_t idx = x + y * bmf->width();
				// " .-:=+*#@";
				const char ramp[] =
					"@MBHENR#KWXDFPQASUZbdehx"
					"*8Gm&04LOVYkpq5Tagns69ow"
					"z$CIu23Jcfry%1v7l+it[] {"
					"}?j|()=~!-/<>\"^_';,:`. ";

				float f = (sizeof(ramp) - 2) * (1 - bmf->bitmap()[idx] / 255.f);

				file << ramp[(size_t) f];
			}
			file << std::endl;
		}
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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);

	test(w0.children() == &w1);
	test(w1.parent()   == &w0);
	test(w2.parent()   == &w0);
	test(w3.parent()   == &w0);

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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);


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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);

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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);

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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);

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

	w0.add(&w1);
	w0.add(&w2);
	w0.add(&w3);

	w2.add(&wa);
	w2.add(&wb);
	w2.add(&wc);


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
