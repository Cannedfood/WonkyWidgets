#include "TreePane.hpp"

#include "../include/wwidget/widget/Button.hpp"
#include "../include/wwidget/Canvas.hpp"

#include "Demangle.hpp"

namespace wwidget {

class Subtree : public List {
	shared<Widget> mWidget;

	List   mHeadBar;
	Button mToggle, mName, mRemove;
public:
	Subtree(shared<Widget> w) :
		mWidget(w),
		mHeadBar(this),
		mToggle(mHeadBar, w->children() ? "(+)" : "( )"),
		mName(mHeadBar),
		mRemove(mHeadBar, "[x]")
	{
		mHeadBar.flow(FlowRight);
		mName.text(demangle(typeid(*w.get()).name()));
		mToggle.onClick([this]() { toggle(); });
		mRemove.onClick([this]() { mWidget->remove(); remove(); });
		mName.onClick([this]() {
			findParent<TreePane>()->signalSelect(mWidget);
		});
	}

	void onDrawBackground(Canvas& c) override {
		if(findParent<TreePane>()->selected() == mWidget)
			c.rect({mHeadBar.width(), mHeadBar.height()}, rgb(87, 87, 87));
	}

	void toggle() {
		if(mHeadBar.nextSibling())
			minimize();
		else
			expand();
	}

	void minimize() {
		if(!mHeadBar.nextSibling()) return;
		mToggle.text("(+)");
		do {
			mHeadBar.nextSibling()->remove();
		} while(mHeadBar.nextSibling());
	}

	void expand() {
		minimize();
		if(!mWidget->children()) {
			mToggle.text("( )");
		}
		else {
			mToggle.text("(-)");
			mWidget->eachChild([this](shared<Widget> child) {
				add<Subtree>(child)->padding(15, 0, 0, 0);
			});
		}
	}
};

TreePane::TreePane() :
	mSelected(nullptr)
{}

void TreePane::setWidget(shared<Widget> w) {
	clearChildren();
	w->eachChild([this](shared<Widget> child) {
		add<Subtree>(child);
	});
}

void TreePane::select(shared<Widget> w) {
	if(mSelected == w) return;
	signalSelect(w);
}

void TreePane::signalSelect(shared<Widget> w) {
	mSelected = w;
	if(onSelect) onSelect(w);
}

} // namespace wwidget
