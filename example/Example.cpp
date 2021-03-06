#include "../include/wwidget/Window.hpp"
#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/widget/List.hpp"
#include "../include/wwidget/widget/ProgressBar.hpp"
#include "../include/wwidget/widget/Slider.hpp"
#include "../include/wwidget/widget/Form.hpp"
#include "../include/wwidget/widget/TextField.hpp"
#include "../include/wwidget/widget/Dialogue.hpp"

#include <cmath>
#include <fstream>

using namespace wwidget;
using namespace std;

static void testWidgetTree();
static void testFont();
static void testUtility();

int chatClient(int argc, char const** argv);

int main(int argc, char const** argv) {
	testWidgetTree();
	// testFont();
	// testUtility();

	Window window(
		"Here goes your title", 800, 600,
		0
		// | Window::FlagUpdateOnEvent
		| Window::FlagAntialias
		| Window::FlagVsync
		// | Window::FlagRelative
	);

	window.add<Form>("example/test.form.xml");

	window.find<Button>("btnSuicide")->onClick([](auto* b) {
		if(b->text() != "Are you sure? [Yes]")
			b->text("Are you sure? [Yes]");
		else
			b->remove();
	});

	window.find<Button>("btnOrientation")->onClick([](Button* b) {
		List* l;
		l = b->findParent<List>("listMain");
		l->flow((wwidget::List::Flow)(l->flow() ^ List::FlowHorizontalBit));
		l = b->findParent<List>();
		l->flow((wwidget::List::Flow)(l->flow() ^ List::FlowHorizontalBit));
		l->align(Widget::AlignCenter, Widget::AlignCenter);
	});
	window.find<Button>("btnDirection")->onClick([](Button* b) {
		List* l;
		l = b->findParent<List>("listMain");
		l->flow((wwidget::List::Flow)(l->flow() ^ List::FlowNegativeBit));
		l = b->findParent<List>();
		l->flow((wwidget::List::Flow)(l->flow() ^ List::FlowNegativeBit));
	});

	window.find<Button>("btnClickerGame")->onClick([](Button* b) {
		auto* p = b->find<ProgressBar>();
		p->progress(p->progress() + 1);
		if(p->progress() > p->scale()) {
			p->progress(p->progress() - p->scale());
			p->scale(p->scale() * 1.2f);
		}
	});

	window.find<Slider>("sldTest")->valueCallback([](Slider* s, float v) {
		std::string c = std::to_string(v);
		while(c.front() == '0') c = c.substr(1);
		while(c.back()  == '0' && c.size() > 2) c.pop_back();
		s->text(c);
	});

	window.find<Button>("btnTestConfirm")
		->onClick([](Button* b) {
			shared<Widget> r = b->findRoot();
			r->add<Dialogue>("Did you want to open this?", [=]() {
				r->add<Dialogue>("You intentionally opened a dialogue!");
			}, nullptr);
		});

	List dumpList;
	dumpList.offset(400, 0);

	window.find<Button>("btnDumpState")->onClick([&](Button* b) {
		shared<Form> form = b->findParent<Form>();
		if(auto file = std::ofstream("structure.tmp.txt"))
			PrintDump(file, *form);
		ShowDump(window, *form)->offset(600, 0)->align(Widget::AlignMax, Widget::AlignMin);
	});

	window.find<TextField>()->onReturn([](TextField* field) {
		auto l = make_shared<Text>();
		field->insertNextSibling(l);
		l->giveOwnershipToParent();
		l->content(field->content());
		field->content("");
	});

	window.add(dumpList);

#ifdef WWIDGET_ULTRA_VERBOSE
	puts("Starting loop");
#endif

	window.keepOpen();

#ifdef WWIDGET_ULTRA_VERBOSE
	puts("Ended loop");
#endif

	if(auto file = std::ofstream("structure.tmp.txt"))
		PrintDump(file, window);

	return 0;
}

#define test(CONDITION) { if(!(CONDITION)) throw std::runtime_error("Line " + std::to_string(__LINE__) + ": Failed test " #CONDITION); }

static void testUtility() {
	// test(Area("10 10") == Area(10, 10));
}

#include "../include/wwidget/Font.hpp"
#include "../include/wwidget/fonts/FontDescription.hpp"

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

				float f = (sizeof(ramp) - 2) * (1 - bmf->data()[idx] / 255.f);

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
