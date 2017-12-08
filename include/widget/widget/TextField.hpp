#include "Label.hpp"

#include <functional>

namespace widget {

// TODO: add cursor
class TextField : public Label {
protected:
	void on(TextInput const& t) override;
	void on(KeyEvent const& k) override;

	void onDraw(Canvas& canvas) override;
	void onUpdate(float dt) override;
	bool onFocus(bool b, float strength) override;
public:
	TextField();
	~TextField();

	std::function<void(TextField* field)> onReturnCallback;
	std::function<void(TextField* field)> onUpdateCallback;

	std::string const& content() const noexcept { return Label::content(); }
	TextField* content(std::string c);
	TextField* onReturn(std::function<void(TextField*)> ret);
	TextField* onUpdate(std::function<void(TextField*)> update);
};

} // namespace widget
