#include "TreePane.hpp"

#include "../include/widget/widget/Button.hpp"

namespace widget {

class Subtree : public List {
	Widget* mWidget;

	List   mHeadBar;
	Button mToggle, mName, mRemove;
public:
	Subtree(Widget* w) :
		mWidget(w),
		mHeadBar(this),
		mToggle(mHeadBar, "(+)"),
		mName(mHeadBar),
		mRemove(mHeadBar, "[x]")
	{
		mHeadBar.flow(FlowRight);
		mName.text(typeid(*w).name());
		mToggle.onClick([this]() { toggle(); });
		mRemove.onClick([this]() { mWidget->remove(); remove(); });
		mName.onClick([this]() { findParent<TreePane>()->select(mWidget); });
	}

	void toggle() {
		if(mHeadBar.nextSibling()) {
			// Minimize
			mToggle.text("(+)");
			do {
				mHeadBar.nextSibling()->remove();
			} while(mHeadBar.nextSibling());
		}
		else {
			mToggle.text("(-)");
			mWidget->eachChild([this](Widget* child) {
				add<Subtree>(child)->padding(15, 0, 0, 0);
			});
		}
	}
};

TreePane::TreePane() {}

void TreePane::setWidget(Widget* w) {
	clearChildren();
	w->eachChild([this](Widget* child) {
		add<Subtree>(child);
	});
}

void TreePane::select(Widget* w) {
	if(onSelect) onSelect(w);
}

} // namespace widget
