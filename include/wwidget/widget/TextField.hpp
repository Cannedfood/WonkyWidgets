#pragma once

#include "Text.hpp"

#include <functional>

namespace wwidget {

// TODO: add cursor
class TextField : public Text {
private:
	std::function<void()> mOnReturn;
	std::function<void()> mOnUpdate;
protected:
	void on(TextInput const& t) override;
	void on(KeyEvent const& k) override;

	void onDraw(Canvas& canvas) override;
	bool onFocus(bool b, FocusType type) override;
public:
	TextField();
	TextField(Widget* addTo);
	~TextField();

	std::string const& content() const noexcept { return Text::content(); }
	TextField* content(std::string c);

	TextField* onReturn(std::function<void()> ret);
	TextField* onUpdate(std::function<void()> update);

	template<class C> std::enable_if_t<std::is_invocable_v<C, TextField*>,
	TextField*> onReturn(C&& c) {
		onReturn(std::function<void()>([this, cc = std::forward<C>(c)]() { cc(this); }));
		return this;
	}
	template<class C> std::enable_if_t<std::is_invocable_v<C, TextField*>,
	TextField*> onUpdate(C&& c) {
		onUpdate(std::function<void()>([this, cc = std::forward<C>(c)]() { cc(this); }));
		return this;
	}
};

} // namespace wwidget
