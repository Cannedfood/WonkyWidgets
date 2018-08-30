#include <wwidget/Widget.hpp>

#include "Test.hpp"

using namespace wwidget;

void testWidgetTreeOps() {
	{
		shared<Widget> a = make_shared<Widget>();
		shared<Widget> child = make_shared<Widget>();
		a->add(child);

		a = make_shared<Widget>();
	}
	return;

	shared<Widget> root, a, b, c;

	auto restore = [&]() {
		if(root) root->clearChildren();

		root = make_shared<Widget>();
		a    = make_shared<Widget>();
		b    = make_shared<Widget>();
		c    = make_shared<Widget>();

		root->add(a);
		root->add(b);
		root->add(c);
	};

	// Test add
	restore();

	expect_eq(root->children(), a);

	expect_eq(a->parent(), root);
	expect_eq(a->prevSibling(), nullptr);
	expect_eq(a->nextSibling(), b);

	expect_eq(b->parent(), root);
	expect_eq(b->prevSibling(), a);
	expect_eq(b->nextSibling(), c);

	expect_eq(c->parent(), root);
	expect_eq(c->prevSibling(), b);
	expect_eq(c->nextSibling(), nullptr);

	// Test remove (part 0/3)
	{
		auto w = make_shared<Widget>();
		expect_eq(w->parent(), nullptr);
		expect_eq(w->nextSibling(), nullptr);
		expect_eq(w->prevSibling(), nullptr);
		w->remove();
		expect_eq(w->parent(), nullptr);
		expect_eq(w->nextSibling(), nullptr);
		expect_eq(w->prevSibling(), nullptr);
	}

	// Test remove (part 1/3)
	restore();

	b->remove();

	expect_eq(b->parent(), nullptr);
	expect_eq(b->nextSibling(), nullptr);
	expect_eq(b->prevSibling(), nullptr);

	expect_eq(a->parent(), root);
	expect_eq(a->prevSibling(), nullptr);
	expect_eq(a->nextSibling(), c);

	expect_eq(c->parent(), root);
	expect_eq(c->prevSibling(), a);
	expect_eq(c->nextSibling(), nullptr);

	// Test remove (part 2/3)
	restore();

	a->remove();

	expect_eq(a->parent(), nullptr);
	expect_eq(a->nextSibling(), nullptr);
	expect_eq(a->prevSibling(), nullptr);

	expect_eq(b->parent(), root);
	expect_eq(b->prevSibling(), nullptr);
	expect_eq(b->nextSibling(), c);

	expect_eq(c->parent(), root);
	expect_eq(c->prevSibling(), b);
	expect_eq(c->nextSibling(), nullptr);
}
