#include <wwidget/Window.hpp>
#include <wwidget/widget/Form.hpp>
#include <wwidget/widget/List.hpp>
#include <wwidget/widget/Button.hpp>
#include <wwidget/widget/Slider.hpp>
#include <wwidget/widget/TextField.hpp>

#include <ctime> // Easier than chrono for what we'll do

using namespace wwidget;

// A simple app for tracking what you were doing and when
class App {
	Window    mWindow;
	List      mMainPane;
	List      mEntries;
	List      mTextFieldLayout;
	TextField mTextField;

public:
	std::string createTimestamp() {
		char result[11] = {'\0'};

		time_t t = time(NULL);
		strftime(result, sizeof(result), "%X", gmtime(&t));

		result[8] = ':';
		result[9] = ' ';
		result[10] = '\0';

		return result;
	}

	void addEntry(std::string const& text) {
		mEntries.add<Label>(createTimestamp() + text);
	}

	void run() {
		mWindow.open("Example 2: SimpleApp", 800, 600, Window::FlagShrinkFit);


		mWindow.add(mMainPane);

		mMainPane.add({
			mEntries, // The entries
			mTextFieldLayout // The stuff for input
		});

		mEntries.scrollable(true);

		mTextFieldLayout.add<Label>("--------->"); // Get some indention
		mTextFieldLayout
			.flow(List::FlowRight)
			->align(Widget::AlignFill, Widget::AlignMin)
			->add(
				mTextField
				.align(Widget::AlignFill, Widget::AlignMin)
			);

		addEntry("Opened the awesome example");

		mTextField.onReturn([&]() {
			addEntry(mTextField.content());
			mTextField.content("");
		});

		mWindow.keepOpen();
	}
};

int main(int argc, char const** argv) {
	App().run();
	return 0;
}
